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

#include "SessionDeserializer.hpp"

#include "data/ActivitySeriesDeserializer.hpp"
#include "data/CompositeDeserializer.hpp"
#include "data/EquipmentDeserializer.hpp"
#include "data/GenericDeserializer.hpp"
#include "data/MeshDeserializer.hpp"
#include "data/PatientDeserializer.hpp"
#include "data/SeriesDeserializer.hpp"
#include "data/StringDeserializer.hpp"
#include "data/StudyDeserializer.hpp"

#include <data/ActivitySeries.hpp>
#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Equipment.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>
#include <data/Mesh.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/Study.hpp>

#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>

namespace sight::io::session
{
namespace detail
{

// Serializer registry
// No concurrency protection as the map is statically initialized
static const std::unordered_map< std::string, std::function<data::IDataDeserializer::uptr(void)> > s_deserializers = {
    {sight::data::Boolean::classname(), & data::GenericDeserializer<sight::data::Boolean>::unique},
    {sight::data::Integer::classname(), & data::GenericDeserializer<sight::data::Integer>::unique},
    {sight::data::Float::classname(), & data::GenericDeserializer<sight::data::Float>::unique},
    {sight::data::String::classname(), & data::StringDeserializer::unique},
    {sight::data::Composite::classname(), & data::CompositeDeserializer::unique},
    {sight::data::Mesh::classname(), & data::MeshDeserializer::unique},
    {sight::data::Equipment::classname(), & data::EquipmentDeserializer::unique},
    {sight::data::Patient::classname(), & data::PatientDeserializer::unique},
    {sight::data::Study::classname(), & data::StudyDeserializer::unique},
    {sight::data::Series::classname(), & data::SeriesDeserializer::unique},
    {sight::data::ActivitySeries::classname(), & data::ActivitySeriesDeserializer::unique}
};

// Return a writer from a data object class name
inline static data::IDataDeserializer::uptr find_deserializer(const std::string& class_name)
{
    const auto& it = s_deserializers.find(class_name);

    if(it != s_deserializers.cend())
    {
        // Return the found writer
        return it->second();
    }

    // Not found return empty one
    return data::IDataDeserializer::uptr();
}

// Implementation class
class SessionDeserializerImpl final : public SessionDeserializer
{
public:
    SIGHT_DECLARE_CLASS(SessionDeserializerImpl, SessionDeserializer)

    /// Delete default copy constructors and assignment operators
    SessionDeserializerImpl(const SessionDeserializerImpl&)            = delete;
    SessionDeserializerImpl(SessionDeserializerImpl&&)                 = delete;
    SessionDeserializerImpl& operator=(const SessionDeserializerImpl&) = delete;
    SessionDeserializerImpl& operator=(SessionDeserializerImpl&&)      = delete;

    /// Default constructor
    SessionDeserializerImpl() = default;

    /// Default destructor
    ~SessionDeserializerImpl() override = default;

    //------------------------------------------------------------------------------

    sight::data::Object::sptr deserialize(
        const std::filesystem::path& archive_path,
        const core::crypto::secure_string& password = "") override
    {
        // Initialize the object cache
        std::map<std::string, sight::data::Object::sptr> cache;

        // Create the archive that contain everything
        const auto& archive = zip::ArchiveReader::shared(archive_path);

        // Create the tree used to store everything and read the index.json from the archive
        boost::property_tree::ptree tree;
        {
            // istream must be closed after this, since archive could only open files one by one
            const auto istream = archive->openFile(this->get_index_file_path(), password);
            boost::property_tree::read_json(*istream, tree);
        }

        SIGHT_THROW_IF(
            "Empty '" << this->get_index_file_path() << "' from archive '" << archive_path << "'.",
                tree.empty());

        return deserialize(cache, archive, tree, password);
    }

private:

    /// Deserializes recursively an initialized archive to a data::Object using an opened property tree
    /// @param cache object cache
    /// @param archive initialized archive
    /// @param tree property tree used to retrieve object index
    /// @param password password to use for optional encryption. Empty password means no encryption
    sight::data::Object::sptr deserialize(
        std::map<std::string, sight::data::Object::sptr>& cache,
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const core::crypto::secure_string& password)
    {
        // First check the cache
        const auto& tree_it     = tree.begin();
        const auto& object_tree = tree_it->second;
        const auto& uuid        = object_tree.get<std::string>("uuid");
        const auto& object_it   = cache.find(uuid);

        if(object_it != cache.cend())
        {
            return object_it->second;
        }
        else
        {
            // Find the serializer using the classname
            const auto& class_name   = tree_it->first;
            const auto& deserializer = find_deserializer(class_name);

            SIGHT_ASSERT(
                "There is no deserializer registered for class '" << class_name << "'.",
                    deserializer);

            // Try to reuse existing rather than create new one
            // Existing object will be overwritten
            auto object = sight::data::Object::dynamicCast(sight::data::Object::fromUUID(uuid));

            // Lock for writing (it will do nothing if object is null)
            sight::data::mt::locked_ptr<sight::data::Object> object_guard(object);

            if(!object)
            {
                // Create the new object so we can safely deserialize child
                object = deserializer->create();
                object->setUUID(uuid);
            }

            // Store the object in cache for later use and to allow circular reference
            cache[uuid] = object;

            // Construct children map, if needed
            std::map<std::string, sight::data::Object::sptr> children;

            const auto& children_it = object_tree.find("children");
            if(children_it != object_tree.not_found())
            {
                for(const auto& child_it : children_it->second)
                {
                    children[child_it.first] = deserialize(
                        cache,
                        archive,
                        child_it.second,
                        password);
                }
            }

            // Now, we can really deserialize the object
            const auto& newObject = deserializer->deserialize(archive, object_tree, children, object, password);

            if(newObject != object)
            {
                // This should not happen normally, only if the serializer doesn't reuse object
                newObject->setUUID(uuid);
                cache[uuid] = newObject;
            }

            // Construct field map
            sight::data::Object::FieldMapType fields;

            const auto& fields_it = object_tree.find("fields");
            if(fields_it != object_tree.not_found())
            {
                for(const auto& field_it : fields_it->second)
                {
                    fields[field_it.first] = deserialize(
                        cache,
                        archive,
                        field_it.second,
                        password);
                }
            }

            // Assign the
            newObject->setFields(fields);

            return newObject;
        }
    }
};

//------------------------------------------------------------------------------

SessionDeserializer::sptr SessionDeserializer::shared()
{
    return std::make_shared<SessionDeserializerImpl>();
}

//------------------------------------------------------------------------------

SessionDeserializer::uptr SessionDeserializer::unique()
{
    return std::make_unique<SessionDeserializerImpl>();
}

} // namespace detail
} // namespace sight::io::session