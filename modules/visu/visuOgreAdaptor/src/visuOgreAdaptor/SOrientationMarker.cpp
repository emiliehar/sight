/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "visuOgreAdaptor/SOrientationMarker.hpp"

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRenderOgre/Layer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <OGRE/OgreCamera.h>

#include <OgreEntity.h>

namespace visuOgreAdaptor
{

static const std::string s_MATRIX_INOUT = "matrix";

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() noexcept
{
}

//-----------------------------------------------------------------------------

SOrientationMarker::~SOrientationMarker() noexcept
{
}

//-----------------------------------------------------------------------------

void SOrientationMarker::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    // Create the transform node.
    this->setTransformId(this->getID() + "_transform");

    // Set the resource this use, if it has been set via xml
    m_patientMeshRc = config.get<std::string>("resource", m_patientMeshRc);

    m_markerDepth = config.get< float > ("depth", m_markerDepth);
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode                            = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    // Sets the material
    m_material = ::fwData::Material::New();

    // Creates the material for the marker
    const ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();
    materialAdaptor->update();

    // Loads and attaches the marker
    m_patientMesh = sceneMgr->createEntity(m_patientMeshRc);
    m_patientMesh->setMaterialName(materialAdaptor->getMaterialName());
    m_sceneNode->attachObject(m_patientMesh);

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::updating()
{
    this->updateCameraMatrix();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SOrientationMarker::updateCameraMatrix()
{

    ::Ogre::Affine3 ogreMatrix;
    {
        const auto transform = this->getLockedInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);

        // Fill the ogreMatrix.
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                ogreMatrix[ct][lt] = static_cast< ::Ogre::Real >(transform->getCoefficient(ct, lt));
            }
        }
    }

    // Get the transformation as sceneNode.
    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);

    // Decompose the matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMatrix.decomposition(position, scale, orientation);

    const ::Ogre::Quaternion rotateX(::Ogre::Degree(180), ::Ogre::Vector3(1, 0, 0));

    // Reset the camera position & orientation, since s_MATRIX_INOUT is a global transform.
    transformNode->setPosition(0, 0, 0);
    // Reverse X axis.
    transformNode->setOrientation(rotateX);

    // Update the camera position
    // Inverse camera matrix (since we move the mesh)
    transformNode->rotate(orientation.Inverse());
    // Place it at a fixed position
    transformNode->translate(0.f, 0.f, m_markerDepth);
}

//-----------------------------------------------------------------------------

void SOrientationMarker::stopping()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::setVisible(bool _visible)
{
    if(m_sceneNode)
    {
        m_sceneNode->setVisible(_visible);
    }

    this->updating();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SOrientationMarker::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
