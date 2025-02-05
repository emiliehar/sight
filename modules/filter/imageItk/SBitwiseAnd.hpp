/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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
 * @brief Implements the AND bitwise operator pixel-wise between two images.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::filter::imageItk::SBitwiseAnd">
           <in key="image" uid="..."/>
           <in key="mask" uid="..."/>
           <out key="outputImage" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::Image]: input image
 * - \b mask [sight::data::Image]: mask
 *
 * @subsection Output Output
 * - \b outputImage [sight::data::Image]: result of operation 'AND' between input image and mask
 */
class MODULE_FILTER_IMAGEITK_CLASS_API SBitwiseAnd final : public sight::service::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SBitwiseAnd, ::sight::service::IOperator);

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API SBitwiseAnd();

    /// Destroys the service.
    MODULE_FILTER_IMAGEITK_API ~SBitwiseAnd() override;

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGEITK_API void stopping() override;

    /// Computes centroids.
    MODULE_FILTER_IMAGEITK_API void updating() override;
};

} // namespace sight::module::filter::imageItk.
