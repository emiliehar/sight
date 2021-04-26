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

#include "ActivitySeriesDeserializer.hpp"

#include "SeriesDeserializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Factories
ActivitySeriesDeserializer::sptr ActivitySeriesDeserializer::shared()
{
    struct make_shared_enabler final : public ActivitySeriesDeserializer {};

    return std::make_shared<make_shared_enabler>();
}

// ------------------------------------------------------------------------------

ActivitySeriesDeserializer::uptr ActivitySeriesDeserializer::unique()
{
    struct make_unique_enabler final : public ActivitySeriesDeserializer {};

    return std::make_unique<make_unique_enabler>();
}

// ------------------------------------------------------------------------------

sight::data::Object::sptr ActivitySeriesDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password)
{
    // Create or reuse the object
    const auto& activitySeries
        = object ? sight::data::ActivitySeries::dynamicCast(object) : sight::data::ActivitySeries::New();

    SIGHT_ASSERT(
        "Object '" << activitySeries->getClassname() << "' is not a '" << sight::data::ActivitySeries::classname() << "'",
            activitySeries);

    // Since ActivitySeries inherits from Series, we could use SeriesDeserializer
    const auto& seriesDeserializer = SeriesDeserializer::unique();
    seriesDeserializer->deserialize(archive, tree, children, activitySeries, password);

    // Deserialize the remaining
    // Deserialize children properties
    activitySeries->setData(sight::data::Composite::dynamicCast(children.at("Data")));

    // Serialize trivial properties
    activitySeries->setActivityConfigId(readFromTree(tree, "ActivityConfigId"));

    return activitySeries;
}

// ------------------------------------------------------------------------------

sight::data::Object::sptr ActivitySeriesDeserializer::create()
{
    // Create the object
    return sight::data::ActivitySeries::New();
}

} // detail::data

} // namespace sight::io::session
