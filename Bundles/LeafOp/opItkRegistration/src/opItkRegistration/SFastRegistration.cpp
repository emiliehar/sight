/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SFastRegistration.hpp"

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <itkRegistrationOp/FastRegistration.hpp>
#include <itkRegistrationOp/Resampler.hpp>

#include <functional>
#include <numeric>

namespace opItkRegistration
{

fwServicesRegisterMacro(::arServices::IRegisterer,
                        ::opItkRegistration::SFastRegistration);

SFastRegistration::SFastRegistration() noexcept :
    ::arServices::IRegisterer()
{
}

//------------------------------------------------------------------------------

SFastRegistration::~SFastRegistration() noexcept
{
}

//------------------------------------------------------------------------------

void SFastRegistration::starting()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::stopping()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::configuring()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::updating()
{
    auto source    = this->getInput< ::fwData::Image>("source");
    auto target    = this->getInput< ::fwData::Image>("target");
    auto transform = this->getInOut< ::fwData::TransformationMatrix3D>("transform");
    SLM_ASSERT("Missing required input 'source'", source);
    SLM_ASSERT("Missing required input 'target'", target);
    SLM_ASSERT("Missing required inout 'transform'", transform);

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.source    = source;
    params.target    = target;
    params.transform = transform;

    {
        ::fwData::mt::ObjectReadLock targetLock(target);
        ::fwData::mt::ObjectReadLock srcLock(source);
        ::fwTools::DynamicType type = target->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ::itkRegistrationOp::RegistrationDispatch >
        ::invoke( type, params );
    }

    transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType>(
        ::fwData::TransformationMatrix3D::s_MODIFIED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SFastRegistration::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace opItkRegistration

