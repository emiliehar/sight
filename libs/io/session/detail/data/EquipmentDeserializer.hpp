/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/session/config.hpp"
#include "io/session/detail/data/IDataDeserializer.hpp"

namespace sight::io::session
{

namespace detail::data
{

/// Class used to serialize composite object to a session
class EquipmentDeserializer : public IDataDeserializer
{
public:
    SIGHT_DECLARE_CLASS(EquipmentDeserializer, IDataDeserializer)
    /// Delete default copy constructors and assignment operators
    EquipmentDeserializer(const EquipmentDeserializer&)            = delete;
    EquipmentDeserializer(EquipmentDeserializer&&)                 = delete;
    EquipmentDeserializer& operator=(const EquipmentDeserializer&) = delete;
    EquipmentDeserializer& operator=(EquipmentDeserializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    ~EquipmentDeserializer() override = default;

    /// Factories
    static EquipmentDeserializer::sptr shared();

    static EquipmentDeserializer::uptr unique();

    // Serialization function
    sight::data::Object::sptr deserialize(
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const std::map<std::string, sight::data::Object::sptr>& children,
        const sight::data::Object::sptr& object,
        const core::crypto::secure_string& password = "") override;

    // Convenience function that returns an empty object of the correct type
    sight::data::Object::sptr create() override;

protected:
    /// Default constructor
    EquipmentDeserializer() = default;
};

} // namespace detail::data

} // namespace sight::io::session
