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
#include "io/session/detail/data/IDataSerializer.hpp"

namespace sight::io::session
{

namespace detail::data
{

/// Class used to serialize a generic object to a session
template<typename T>
class GenericSerializer : public IDataSerializer
{
public:
    SIGHT_DECLARE_CLASS(GenericSerializer<T>, IDataSerializer)
    /// Delete default copy constructors and assignment operators
    GenericSerializer(const GenericSerializer&)            = delete;
    GenericSerializer(GenericSerializer&&)                 = delete;
    GenericSerializer& operator=(const GenericSerializer&) = delete;
    GenericSerializer& operator=(GenericSerializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    ~GenericSerializer() override = default;

    /// Factories
    inline static GenericSerializer::sptr shared();

    inline static GenericSerializer::uptr unique();

    /// Serialization function
    inline void serialize(
        const zip::ArchiveWriter::sptr& archive,
        boost::property_tree::ptree& tree,
        const sight::data::Object::csptr& object,
        std::map<std::string, sight::data::Object::csptr>& children,
        const core::crypto::secure_string& password = "") override;

protected:
    /// Default constructor
    GenericSerializer() = default;
};

// ------------------------------------------------------------------------------

template<typename T>
typename GenericSerializer<T>::sptr GenericSerializer<T>::shared()
{
    struct make_shared_enabler final : public GenericSerializer<T> {};

    return std::make_shared<make_shared_enabler>();
}

// ------------------------------------------------------------------------------

template<typename T>
typename GenericSerializer<T>::uptr GenericSerializer<T>::unique()
{
    struct make_unique_enabler final : public GenericSerializer<T> {};

    return std::make_unique<make_unique_enabler>();
}

// ------------------------------------------------------------------------------

template<typename T>
void GenericSerializer<T>::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password)
{
    const auto& newObject = T::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
            << (object ? object->getClassname() : sight::data::Object::classname())
            << "' is not a '"
            << T::classname()
            << "'",
            newObject);

    tree.put("Value", newObject->getValue());
}

} // namespace detail::data

} // namespace sight::io::session
