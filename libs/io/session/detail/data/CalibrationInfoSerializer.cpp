/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "CalibrationInfoSerializer.hpp"

#include <data/CalibrationInfo.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void CalibrationInfoSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password
) const
{
    const auto& calibrationInfo = sight::data::CalibrationInfo::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
        << (object ? object->getClassname() : sight::data::Object::classname())
        << "' is not a '"
        << sight::data::CalibrationInfo::classname()
        << "'",
        calibrationInfo
    );

    // Add a version number. Not mandatory, but could help for future release
    tree.put("version", 1);

    // Images
    int index = 0;
    for(const auto& image : calibrationInfo->getImageContainer())
    {
        children[image->getClassname() + std::to_string(index++)] = image;
    }

    // PointLists
    index = 0;
    for(const auto& pointList : calibrationInfo->getPointListContainer())
    {
        children[pointList->getClassname() + std::to_string(index++)] = pointList;
    }
}

} // detail::data

} // namespace sight::io::session
