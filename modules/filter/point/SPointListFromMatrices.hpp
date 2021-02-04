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

#include "modules/filter/point/config.hpp"

#include <core/com/Signal.hpp>

#include <data/TransformationMatrix3D.hpp>

#include <services/IOperator.hpp>

namespace sight::modules::filter::point
{

/**
 * @brief The SPointListFromMatrices service extracts translation part of multiple matrices and put it into a pointlist.
 * The pointlist is cleared by default. It can be set to 'appending' mode through the configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::sight::modules::filter::point::SPointListFromMatrices">
            <in group="matrices">
                <key uid="matrix1" />
                <key uid="matrix2" />
                <key uid="matrix3" />
            </in>
            <inout key="pointList"/>
            <config append="false"/>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrices [data::TransformationMatrix3D]: input matrices (only translation part will be extracted).
 *
 * @subsection Inout Inout
 * - \b pointList [data::PointList]: output pointlist, each point represent translation part of corresponding matrix
 *
 * @subsection Configuration Configuration
 * - \b append (optional, default=false): does the service append points in the pointlist on update (true) or clear it
 * (false)?
 */
class MODULE_FILTER_POINT_CLASS_API SPointListFromMatrices : public services::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SPointListFromMatrices, services::IOperator)

    /// Initializes a member variable
    MODULE_FILTER_POINT_API SPointListFromMatrices();

    /// Destroys the service
    MODULE_FILTER_POINT_API virtual ~SPointListFromMatrices();

protected:

    /// Sets the append mode on or off.
    MODULE_FILTER_POINT_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_POINT_API void starting() override;

    /// Fills the PointList.
    MODULE_FILTER_POINT_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_POINT_API void stopping() override;

private:

    /// Append mode
    bool m_append;
};

} // namespace sight::modules::filter::point.
