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

#include "SessionSerializer.hpp"

#include "data/ActivitySeriesSerializer.hpp"
#include "data/CompositeSerializer.hpp"
#include "data/EquipmentSerializer.hpp"
#include "data/GenericSerializer.hpp"
#include "data/MeshSerializer.hpp"
#include "data/PatientSerializer.hpp"
#include "data/SeriesSerializer.hpp"
#include "data/StringSerializer.hpp"
#include "data/StudySerializer.hpp"

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

#include <io/zip/ArchiveWriter.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

namespace detail
{

// Serializer registry
// No concurrency protection as the map is statically initialized
static const std::unordered_map<std::string, std::function<data::IDataSerializer::uptr(void)> > s_serializers = {
    {sight::data::Boolean::classname(), &data::GenericSerializer<sight::data::Boolean>::unique},
    {sight::data::Integer::classname(), &data::GenericSerializer<sight::data::Integer>::unique},
    {sight::data::Float::classname(), &data::GenericSerializer<sight::data::Float>::unique},
    {sight::data::String::classname(), &data::StringSerializer::unique},
    {sight::data::Composite::classname(), &data::CompositeSerializer::unique},
    {sight::data::Mesh::classname(), &data::MeshSerializer::unique},
    {sight::data::Equipment::classname(), &data::EquipmentSerializer::unique},
    {sight::data::Patient::classname(), &data::PatientSerializer::unique},
    {sight::data::Study::classname(), &data::StudySerializer::unique},
    {sight::data::Series::classname(), &data::SeriesSerializer::unique},
    {sight::data::ActivitySeries::classname(), &data::ActivitySeriesSerializer::unique}
};

// Return a writer from a data object class name
inline static data::IDataSerializer::uptr find_serializer(const std::string& class_name)
{
    const auto& it = s_serializers.find(class_name);

    if(it != s_serializers.cend())
    {
        // Return the found writer
        return it->second();
    }

    // Not found return empty one
    return data::IDataSerializer::uptr();
}

// Implementation class
class SessionSerializerImpl final : public SessionSerializer
{
public:
    SIGHT_DECLARE_CLASS(SessionSerializerImpl, SessionSerializer)
    /// Delete default copy constructors and assignment operators
    SessionSerializerImpl(const SessionSerializerImpl&)            = delete;
    SessionSerializerImpl(SessionSerializerImpl&&)                 = delete;
    SessionSerializerImpl& operator=(const SessionSerializerImpl&) = delete;
    SessionSerializerImpl& operator=(SessionSerializerImpl&&)      = delete;

    /// Default constructor
    SessionSerializerImpl() = default;

    /// Default destructor
    ~SessionSerializerImpl() override = default;

    // ------------------------------------------------------------------------------

    void serialize(
        const std::filesystem::path& archive_path,
        const sight::data::Object::csptr& object,
        const core::crypto::secure_string& password = "") override
    {
        // Initialize the ptree cache
        std::set<std::string> cache;

        // Create the archive that will hold the property tree and all binary files
        const auto& archive = zip::ArchiveWriter::shared(archive_path);

        // Create the tree used to store indexes.
        boost::property_tree::ptree tree;

        // Serialize recursively everything into the tree and the archive
        serialize(cache, archive, tree, object, password);

        auto ostream = archive->openFile(this->get_index_file_path(), password, zip::Method::ZSTD, zip::Level::BEST);

        // Write the final property tree back to the archive
        boost::property_tree::write_json(
            *ostream,
            tree,
            false);
    }

private:
    /// Serializes recursively a data::Object to an opened archive using an initialized property tree
    /// @param cache ptree cache
    /// @param archive opened archive
    /// @param tree property tree used to store object index
    /// @param object root object to serialize
    /// @param password password to use for optional encryption. Empty password means no encryption
    void serialize(
        std::set<std::string>& cache,
        const zip::ArchiveWriter::sptr& archive,
        boost::property_tree::ptree& tree,
        const sight::data::Object::csptr& object,
        const core::crypto::secure_string& password)
    {
        // Lock the object
        sight::data::mt::locked_ptr<const sight::data::Object> lock(object);

        // First check the cache
        const auto& uuid       = object->getUUID();
        const auto& uuid_it    = cache.find(uuid);
        const auto& class_name = object->getClassname();

        if(uuid_it != cache.cend())
        {
            boost::property_tree::ptree cached_tree;
            cached_tree.put("uuid", uuid);
            tree.add_child(class_name, cached_tree);
        }
        else
        {
            // Store uuid in cache immediately to allow circular reference
            cache.insert(uuid);

            // Create the object ptree
            boost::property_tree::ptree object_tree;

            // Put basic meta information
            object_tree.put("uuid", uuid);

            // Find the serializer using the classname
            const auto& serializer = find_serializer(class_name);

            SIGHT_ASSERT(
                "There is no serializer registered for class '" << class_name << "'.",
                    serializer);

            // Ask the serializer to serialize
            std::map<std::string, sight::data::Object::csptr> children;

            serializer->serialize(
                archive,
                object_tree,
                object,
                children,
                password);

            // Serialize children, if needed
            if(!children.empty())
            {
                boost::property_tree::ptree children_tree;

                for(const auto& child : children)
                {
                    boost::property_tree::ptree child_tree;

                    // Recursively serialize child objects
                    serialize(cache, archive, child_tree, child.second, password);

                    // Append to the children tree
                    children_tree.add_child(child.first, child_tree);
                }

                // Add children tree
                object_tree.add_child("children", children_tree);
            }

            // Serialize fields, if needed
            const auto& fields = object->getFields();

            if(!fields.empty())
            {
                boost::property_tree::ptree fields_tree;

                for(const auto& field : fields)
                {
                    // Recursively serialize field object
                    boost::property_tree::ptree field_tree;
                    serialize(cache, archive, field_tree, field.second, password);

                    // Append to the fields tree
                    fields_tree.add_child(field.first, field_tree);
                }

                // Add fields tree
                object_tree.add_child("fields", fields_tree);
            }

            // Add the new tree to the root
            tree.add_child(class_name, object_tree);
        }
    }
};

// ------------------------------------------------------------------------------

SessionSerializer::sptr SessionSerializer::shared()
{
    return std::make_shared<SessionSerializerImpl>();
}

// ------------------------------------------------------------------------------

SessionSerializer::uptr SessionSerializer::unique()
{
    return std::make_unique<SessionSerializerImpl>();
}

} // namespace detail

} // namespace sight::io::session
