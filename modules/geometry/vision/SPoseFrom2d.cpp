/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SPoseFrom2d.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/Camera.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/FrameTL.hpp>
#include <data/MarkerTL.hpp>
#include <data/Matrix4.hpp>
#include <data/MatrixTL.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Camera.hpp>

//-----------------------------------------------------------------------------

namespace sight::module::geometry::vision
{

//-----------------------------------------------------------------------------

const service::IService::KeyType s_MARKERTL_INPUT  = "markerTL";
const service::IService::KeyType s_MARKERMAP_INPUT = "markerMap";
const service::IService::KeyType s_CAMERA_INPUT    = "camera";
const service::IService::KeyType s_EXTRINSIC_INPUT = "extrinsic";
const service::IService::KeyType s_MATRIXTL_INOUT  = "matrixTL";
const service::IService::KeyType s_MATRIX_INOUT    = "matrix";
const service::IService::KeyType s_POINTLIST_INOUT = "pointList";

//-----------------------------------------------------------------------------

SPoseFrom2d::SPoseFrom2d() noexcept :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_isInitialized(false)
{
}

//-----------------------------------------------------------------------------

SPoseFrom2d::~SPoseFrom2d() noexcept
{
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();
    m_patternWidth = config.get<double>("patternWidth", m_patternWidth);
    SIGHT_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);

    auto inoutCfg = config.equal_range("inout");
    for(auto itCfg = inoutCfg.first ; itCfg != inoutCfg.second ; ++itCfg)
    {
        const auto group = itCfg->second.get<std::string>("<xmlattr>.group");
        if(group == s_MATRIX_INOUT)
        {
            auto keyCfg = itCfg->second.equal_range("key");
            for(auto itKeyCfg = keyCfg.first ; itKeyCfg != keyCfg.second ; ++itKeyCfg)
            {
                const data::MarkerMap::KeyType key = itKeyCfg->second.get<std::string>("<xmlattr>.id");
                m_matricesTag.push_back(key);
            }

            break;
        }
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::starting()
{
    //3D Points
    const float halfWidth = static_cast<float>(m_patternWidth) * .5f;

    m_3dModel.push_back(::cv::Point3f(-halfWidth, halfWidth, 0));
    m_3dModel.push_back(::cv::Point3f(halfWidth, halfWidth, 0));
    m_3dModel.push_back(::cv::Point3f(halfWidth, -halfWidth, 0));
    m_3dModel.push_back(::cv::Point3f(-halfWidth, -halfWidth, 0));

    data::PointList::sptr pl = this->getInOut<data::PointList>(s_POINTLIST_INOUT);
    if(pl)
    {
        data::mt::ObjectWriteLock lock(pl);
        for(size_t i = 0 ; i < m_3dModel.size() ; ++i)
        {
            const ::cv::Point3f cvPoint    = m_3dModel.at(i);
            const data::Point::sptr point  = data::Point::New(cvPoint.x, cvPoint.y, cvPoint.z);
            const data::String::sptr label = data::String::New(std::to_string(i));
            point->setField(data::fieldHelper::Image::m_labelId, label);
            pl->pushBack(point);
        }

        auto sig = pl->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::stopping()
{
    m_cameras.clear();
    m_3dModel.clear();
    m_lastTimestamp = 0;
    m_isInitialized = false;

    data::PointList::sptr pl = this->getInOut<data::PointList>(s_POINTLIST_INOUT);
    if(pl)
    {
        data::mt::ObjectWriteLock lock(pl);
        pl->clear();
        auto sig = pl->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::HiResClock::getTimeInMilliSec();
    this->computeRegistration(timestamp);
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::computeRegistration(core::HiResClock::HiResClockType timestamp)
{
    SIGHT_WARN_IF("Invoking doRegistration while service is STOPPED", this->isStopped());

    if(!m_isInitialized)
    {
        this->initialize();
    }

    if(this->isStarted())
    {
        if(this->getKeyGroupSize(s_MARKERTL_INPUT) > 0)
        {
            if(timestamp > m_lastTimestamp)
            {
                core::HiResClock::HiResClockType newerTimestamp =
                    std::numeric_limits<core::HiResClock::HiResClockType>::max();
                for(size_t i = 0 ; i < this->getKeyGroupSize(s_MARKERTL_INPUT) ; ++i)
                {
                    auto markerTL = this->getInput<data::MarkerTL>(
                        s_MARKERTL_INPUT,
                        i
                    );
                    core::HiResClock::HiResClockType timestamp = markerTL->getNewerTimestamp();
                    if(timestamp <= 0.)
                    {
                        SIGHT_WARN("No marker found in a timeline for timestamp '" << timestamp << "'.");
                        return;
                    }

                    newerTimestamp = std::min(timestamp, newerTimestamp);
                }

                m_lastTimestamp = newerTimestamp;

                // We consider that all timeline have the same number of elements
                // This is WRONG if we have more than two cameras
                auto markerTL = this->getInput<data::MarkerTL>(s_MARKERTL_INPUT, 0);
                const CSPTR(data::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);
                const unsigned int numMarkers = buffer->getMaxElementNum();

                data::MatrixTL::sptr matrixTL = this->getInOut<data::MatrixTL>(s_MATRIXTL_INOUT);

                // Push matrix in timeline
                SPTR(data::MatrixTL::BufferType) matrixBuf;

                // For each marker
                bool matrixBufferCreated = false;
                for(unsigned int markerIndex = 0 ; markerIndex < numMarkers ; ++markerIndex)
                {
                    std::vector<Marker> markers;
                    size_t indexTL = 0;

                    // For each camera timeline
                    for(size_t i = 0 ; i < this->getKeyGroupSize(s_MARKERTL_INPUT) ; ++i)
                    {
                        auto markerTL = this->getInput<data::MarkerTL>(s_MARKERTL_INPUT, i);
                        const CSPTR(data::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);

                        if(buffer->isPresent(markerIndex))
                        {
                            const float* registrationCVBuffer = buffer->getElement(markerIndex);

                            Marker currentMarker;
                            for(size_t i = 0 ; i < 4 ; ++i)
                            {
                                currentMarker.corners2D.push_back(
                                    ::cv::Point2f(
                                        registrationCVBuffer[i * 2],
                                        registrationCVBuffer[i * 2 + 1]
                                    )
                                );
                            }

                            markers.push_back(currentMarker);
                        }

                        ++indexTL;
                    }

                    if(markers.empty())
                    {
                        SIGHT_WARN("No Markers!")
                        continue;
                    }

                    float matrixValues[16];
                    ::cv::Matx44f Rt;
                    if(markers.size() == 1)
                    {
                        Rt = this->cameraPoseFromMono(markers[0]);
                    }
                    else if(markers.size() == 2)
                    {
                        Rt = this->cameraPoseFromStereo(markers[0], markers[1]);
                    }
                    else
                    {
                        SIGHT_WARN("More than 2 cameras is not handle for the moment");
                        continue;
                    }

                    for(std::uint8_t i = 0 ; i < 4 ; ++i)
                    {
                        for(std::uint8_t j = 0 ; j < 4 ; ++j)
                        {
                            matrixValues[4 * i + j] = Rt(i, j);
                        }
                    }

                    if(!matrixBufferCreated)
                    {
                        matrixBuf = matrixTL->createBuffer(newerTimestamp);
                        matrixTL->pushObject(matrixBuf);
                        matrixBufferCreated = true;
                    }

                    matrixBuf->setElement(matrixValues, markerIndex);
                }

                if(matrixBufferCreated)
                {
                    //Notify
                    data::TimeLine::ObjectPushedSignalType::sptr sig;
                    sig = matrixTL->signal<data::TimeLine::ObjectPushedSignalType>(
                        data::TimeLine::s_OBJECT_PUSHED_SIG
                    );

                    sig->asyncEmit(newerTimestamp);
                }
            }
        }
        else
        {
            // For each marker
            unsigned int markerIndex = 0;
            for(auto markerKey : m_matricesTag)
            {
                std::vector<Marker> markers;
                size_t indexTL = 0;

                // For each camera timeline
                for(size_t i = 0 ; i < this->getKeyGroupSize(s_MARKERMAP_INPUT) ; ++i)
                {
                    auto markerMap = this->getInput<data::MarkerMap>(s_MARKERMAP_INPUT, i);
                    data::mt::ObjectReadLock lock(markerMap);
                    const auto* marker = markerMap->getMarker(markerKey);

                    if(marker)
                    {
                        Marker currentMarker;
                        for(size_t i = 0 ; i < 4 ; ++i)
                        {
                            currentMarker.corners2D.push_back(::cv::Point2f((*marker)[i][0], (*marker)[i][1]));
                        }

                        markers.push_back(currentMarker);
                    }

                    ++indexTL;
                }

                data::Matrix4::sptr matrix = this->getInOut<data::Matrix4>(
                    s_MATRIX_INOUT,
                    markerIndex
                );
                SIGHT_ASSERT("Matrix " << markerIndex << " not found", matrix);
                if(markers.empty())
                {
                    SIGHT_WARN("No Markers!")
                }
                else
                {
                    data::Matrix4::TMCoefArray matrixValues;
                    ::cv::Matx44f Rt;
                    if(markers.size() == 1)
                    {
                        Rt = this->cameraPoseFromMono(markers[0]);
                    }
                    else if(markers.size() == 2)
                    {
                        Rt = this->cameraPoseFromStereo(markers[0], markers[1]);
                    }
                    else
                    {
                        SIGHT_WARN("More than 2 cameras is not handle for the moment");
                        continue;
                    }

                    for(std::uint8_t i = 0 ; i < 4 ; ++i)
                    {
                        for(std::uint8_t j = 0 ; j < 4 ; ++j)
                        {
                            matrixValues[4 * i + j] = Rt(i, j);
                        }
                    }

                    data::mt::ObjectWriteLock lock(matrix);
                    matrix->setCoefficients(matrixValues);
                }

                // Always send the signal even if we did not find anything.
                // This allows to keep updating the whole processing pipeline.
                auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                sig->asyncEmit();

                ++markerIndex;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::initialize()
{
    if(this->getKeyGroupSize(s_MARKERTL_INPUT) > 0)
    {
        // Initialization of timelines
        data::MarkerTL::csptr firstTimeline = this->getInput<data::MarkerTL>(s_MARKERTL_INPUT, 0);

        const unsigned int maxElementNum = firstTimeline->getMaxElementNum();

        for(size_t i = 0 ; i < this->getKeyGroupSize(s_MARKERTL_INPUT) ; ++i)
        {
            data::MarkerTL::csptr timeline = this->getInput<data::MarkerTL>(s_MARKERTL_INPUT, 0);

            SIGHT_ASSERT(
                "Timelines should have the same maximum number of elements",
                maxElementNum == timeline->getMaxElementNum()
            );
        }

        data::MatrixTL::sptr matrixTL = this->getInOut<data::MatrixTL>(s_MATRIXTL_INOUT);
        // initialized matrix timeline
        matrixTL->initPoolSize(maxElementNum);
    }

    for(size_t idx = 0 ; idx < this->getKeyGroupSize(s_CAMERA_INPUT) ; ++idx)
    {
        data::Camera::csptr camera = this->getInput<data::Camera>(s_CAMERA_INPUT, idx);
        SIGHT_FATAL_IF("Camera[" << idx << "] not found", !camera);
        data::mt::ObjectReadLock cameraLock(camera);

        Camera cam;
        std::tie(cam.intrinsicMat, cam.imageSize, cam.distCoef) = io::opencv::Camera::copyToCv(camera);

        // set extrinsic matrix only if stereo.
        if(idx == 1)
        {
            auto extrinsicMatrix = this->getInput<data::Matrix4>(s_EXTRINSIC_INPUT);
            data::mt::ObjectReadLock matrixLock(extrinsicMatrix);

            SIGHT_FATAL_IF("Extrinsic matrix with key '" + s_EXTRINSIC_INPUT + "' not found", !extrinsicMatrix);

            m_extrinsic.Matrix4x4   = ::cv::Mat::eye(4, 4, CV_64F);
            m_extrinsic.rotation    = ::cv::Mat::eye(3, 3, CV_64F);
            m_extrinsic.translation = ::cv::Mat::eye(3, 1, CV_64F);

            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                for(std::uint8_t j = 0 ; j < 3 ; ++j)
                {
                    m_extrinsic.rotation.at<double>(i, j)  = extrinsicMatrix->getCoefficient(i, j);
                    m_extrinsic.Matrix4x4.at<double>(i, j) = extrinsicMatrix->getCoefficient(i, j);
                }
            }

            m_extrinsic.translation.at<double>(0, 0) = extrinsicMatrix->getCoefficient(0, 3);
            m_extrinsic.translation.at<double>(1, 0) = extrinsicMatrix->getCoefficient(1, 3);
            m_extrinsic.translation.at<double>(2, 0) = extrinsicMatrix->getCoefficient(2, 3);

            m_extrinsic.Matrix4x4.at<double>(0, 3) = extrinsicMatrix->getCoefficient(0, 3);
            m_extrinsic.Matrix4x4.at<double>(1, 3) = extrinsicMatrix->getCoefficient(1, 3);
            m_extrinsic.Matrix4x4.at<double>(2, 3) = extrinsicMatrix->getCoefficient(2, 3);
        }

        m_cameras.push_back(cam);
    }

    m_isInitialized = true;
}

//-----------------------------------------------------------------------------

const cv::Matx44f SPoseFrom2d::cameraPoseFromStereo(
    const SPoseFrom2d::Marker& _markerCam1,
    const SPoseFrom2d::Marker& _markerCam2
) const
{
    ::cv::Matx44f pose = sight::geometry::vision::helper::cameraPoseStereo(
        m_3dModel,
        m_cameras[0].intrinsicMat,
        m_cameras[0].distCoef,
        m_cameras[1].intrinsicMat,
        m_cameras[1].distCoef,
        _markerCam1.corners2D,
        _markerCam2.corners2D,
        m_extrinsic.rotation,
        m_extrinsic.translation
    );

    return pose;
}

//-----------------------------------------------------------------------------

const cv::Matx44f SPoseFrom2d::cameraPoseFromMono(const SPoseFrom2d::Marker& _markerCam1) const
{
    ::cv::Matx44f pose =
        sight::geometry::vision::helper::cameraPoseMonocular(
            m_3dModel,
            _markerCam1.corners2D,
            m_cameras[0].intrinsicMat,
            m_cameras[0].distCoef
        );
    return pose;
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SPoseFrom2d::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_MARKERTL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, s_COMPUTE_REGISTRATION_SLOT);
    connections.push(s_MARKERMAP_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
