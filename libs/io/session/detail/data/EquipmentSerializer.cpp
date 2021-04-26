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

#include "EquipmentSerializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Equipment.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Factories
EquipmentSerializer::sptr EquipmentSerializer::shared()
{
    struct make_shared_enabler final : public EquipmentSerializer {};

    return std::make_shared<make_shared_enabler>();
}

// ------------------------------------------------------------------------------

EquipmentSerializer::uptr EquipmentSerializer::unique()
{
    struct make_unique_enabler final : public EquipmentSerializer {};

    return std::make_unique<make_unique_enabler>();
}

/// Serialization function
void EquipmentSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password)
{
    const auto& equipment = sight::data::Equipment::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
            << (object ? object->getClassname() : sight::data::Object::classname())
            << "' is not a '"
            << sight::data::Equipment::classname()
            << "'",
            equipment);

    writeToTree(tree, "InstitutionName", equipment->getInstitutionName());
}

} // detail::data

} // namespace sight::io::session
