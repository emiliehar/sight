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

#include "ILocation.hpp"

#include <filesystem>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace sight::core::location
{
/// Class to define a location that holds one or more file path.
class CORE_CLASS_API MultipleFiles : public core::location::ILocation
{
public:
    SIGHT_DECLARE_CLASS(MultipleFiles)

    /// Factory
    inline static MultipleFiles::sptr New();

    /// String serialization function
    inline std::string toString() const override;

    /// Sets the file paths
    inline void setFiles(const std::vector<std::filesystem::path>& filePaths);

    /// Gets the file paths
    inline std::vector<std::filesystem::path> getFiles() const;

    /// Append one file path
    inline void addFile(const std::filesystem::path& filePaths);

protected:

    /// Constructor
    CORE_API MultipleFiles() = default;

    /// Destructor
    CORE_API virtual ~MultipleFiles() = default;

private:

    /// The filesystem::path vector
    std::vector<std::filesystem::path> m_files;
};

// Factory
inline MultipleFiles::sptr MultipleFiles::New()
{
    struct MakeSharedEnabler : public MultipleFiles {};
    return std::make_shared<MakeSharedEnabler>();
}

// String serialization function
inline std::string MultipleFiles::toString() const
{
    #ifdef WIN32
    const char* const delim = ";";
    #else
    const char* const delim = ":";
    #endif

    std::stringstream stringstream;
    std::copy(m_files.begin(), m_files.end(), std::ostream_iterator<std::string>(stringstream, delim));
    return stringstream.str();
}

//------------------------------------------------------------------------------

inline void MultipleFiles::setFiles(const std::vector<std::filesystem::path>& filePaths)
{
    m_files = filePaths;
}

//------------------------------------------------------------------------------

inline std::vector<std::filesystem::path> MultipleFiles::getFiles() const
{
    return m_files;
}

//------------------------------------------------------------------------------

inline void MultipleFiles::addFile(const std::filesystem::path& filePaths)
{
    m_files.push_back(filePaths);
}

} // namespace sight::io::base::location
