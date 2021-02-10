/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <uiVisuOgre/config.hpp>

#include <data/PointList.hpp>
#include <data/tools/PickingInfo.hpp>

#include <services/IController.hpp>

namespace uiVisuOgre
{

/**
 * @brief   Add points in a data::PointList
 *
 * @section Slots Slots
 * - \b pick(data::tools::PickingInfo) : Add or remove the closest point to the picking position, removal occurs when
 * `CTRL` is pressed.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiVisuOgre::SAddPoint">
            <inout key="pointList" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b pointList [sight::data::PointList]: target points list.
 */
class UIVISUOGRE_CLASS_API SAddPoint : public services::IController
{
public:

    fwCoreServiceMacro(SAddPoint, services::IController)

    /**
     * @name Slots API
     * @{
     */

    /// SLOT: Use data from the picking info to add or remove a point.
    UIVISUOGRE_API static const core::com::Slots::SlotKeyType s_PICK_SLOT;

    /// SLOT: Clear the point list.
    UIVISUOGRE_API static const core::com::Slots::SlotKeyType s_CLEAR_POINTS_SLOT;
    /** @} */

    /// Constructor. Creates slot.
    UIVISUOGRE_API SAddPoint() noexcept;

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SAddPoint() noexcept;

protected:

    UIVISUOGRE_API virtual void configuring() override;

    /// Does nothing
    UIVISUOGRE_API virtual void starting() override;

    /// Does nothing
    UIVISUOGRE_API virtual void stopping() override;

    /// Does nothing
    UIVISUOGRE_API virtual void updating() override;

private:

    ///  Add a point in the point list
    void addPoint(const data::Point::sptr);

    ///  Remove a point from the point list
    void removePoint(const data::Point::csptr);

    /**
     * @name Slots methods
     * @{
     */

    /// SLOT: Add or remove a point.
    void pick(data::tools::PickingInfo);

    /// SLOT: Clear the point list.
    void clearPoints();

    /**
     * @}
     */
};
} // uiVisuOgre
