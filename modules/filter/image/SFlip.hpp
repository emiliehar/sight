/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#pragma once

#include "modules/filter/image/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Slots.hpp>

#include <services/IOperator.hpp>

namespace sight::modules::filter::image
{

/**
 * @brief Service flipping an input image along x or y or z axis.
 *
 * @section Slots Slots
 * - flipAxisX(): flip along X axis
 * - flipAxisY(): flip along Y axis
 * - flipAxisZ(): flip along Z axis

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="::sight::modules::filter::image::SFlip">
           <in key="source" uid="..." autoConnect="yes" />
           <out key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input
 * - \b source [sight::data::Image]: Image to flip.
 * @subsection Output Output:
 * - \b target [sight::data::Image]: New flipped image.
 */
class MODULE_FILTER_IMAGE_CLASS_API SFlip : public services::IOperator
{
public:

    fwCoreServiceMacro(SFlip, ::sight::services::IOperator)

    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXISX_SLOT;
    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXISY_SLOT;
    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXISZ_SLOT;

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGE_API SFlip();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGE_API ~SFlip();

protected:

    /// Configures the service.
    MODULE_FILTER_IMAGE_API virtual void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API virtual void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API virtual void stopping() override;

    /// Apply the flip operator.
    MODULE_FILTER_IMAGE_API virtual void updating() override;

    /**
     * @brief Signal-slot auto-connection proposals
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_FILTER_IMAGE_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: flip the first axis
    void flipAxisX();
    /// Slot: flip the second axis
    void flipAxisY();
    /// Slot: flip the third axis
    void flipAxisZ();

    // Store whether to flip or not one of the 3 axis
    std::array<bool, 3> m_flipAxes {{false, false, false}};
};

} // namespace sight::modules::filter::image
