/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "modules/filter/imageItk/config.hpp"

#include <service/IOperator.hpp>

namespace sight::module::filter::imageItk
{

/**
 * @brief Service applying a transform to an image and outputing the resampled image.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after resampling.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="opITK::SPropagator">
       <in key="imageIn" uid="..." autoConnect="true" />
       <in key="transform" uid="..." />
       <in key="target" uid="..." />
       <inout key="imageOut" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [sight::data::Image]: Image to resample.
 * - \b transform [sight::data::Matrix4]: Transform to apply.
 * - \b target [sight::data::Image] (optional): target image defining the size, spacing and origin of the output.
 * @subsection In-Out In-Out
 * - \b imageOut [sight::data::Image]: New resampled image.
 */
class MODULE_FILTER_IMAGEITK_CLASS_API SResampler : public service::IOperator
{
public:

    SIGHT_DECLARE_SERVICE(SResampler, service::IOperator);

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGEITK_API SResampler();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGEITK_API ~SResampler();

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void stopping() override;

    /// Apply the transform and resample.
    MODULE_FILTER_IMAGEITK_API void updating() override;

    /**
     * @brief Auto connections
     *
     * - Update service when the image or its buffer is modified.
     * - Update service when the transform matrix is modified.
     * - Update service when the target image is modified.
     */
    MODULE_FILTER_IMAGEITK_API KeyConnectionsMap getAutoConnections() const override;
};

} // namespace sight::module::filter::imageItk
