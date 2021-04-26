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

#include "ArchiveWriter.hpp"

#include "exception/Write.hpp"

#include "minizip/zip.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

#include <tuple>

namespace sight::io::zip
{

/// Convert argument to minizip dialect
static inline std::tuple<int, int> to_minizip_parameter(Method method, Level level)
{
    int minizip_method = MZ_COMPRESS_METHOD_ZSTD;
    int minizip_level  = MZ_COMPRESS_LEVEL_DEFAULT;

    switch(method)
    {
        case Method::STORE:
            minizip_method = MZ_COMPRESS_METHOD_STORE;
            minizip_level  = MZ_COMPRESS_LEVEL_DEFAULT;
            break;

        case Method::DEFLATE:
            minizip_method = MZ_COMPRESS_METHOD_DEFLATE;

            switch(level)
            {
                case Level::BEST:
                case Level::ULTRA:
                    minizip_level = MZ_COMPRESS_LEVEL_BEST;
                    break;

                case Level::FAST:
                    minizip_level = MZ_COMPRESS_LEVEL_FAST;
                    break;

                default:
                    minizip_level = MZ_COMPRESS_LEVEL_DEFAULT;
                    break;
            }

            break;

        case Method::ZSTD:
            minizip_method = MZ_COMPRESS_METHOD_ZSTD;

            switch(level)
            {
                case Level::ULTRA:
                    minizip_level = 22;
                    break;

                case Level::BEST:
                    minizip_level = 19;
                    break;

                case Level::FAST:
                    minizip_level = 1;
                    break;

                default:
                    minizip_level = 5;
                    break;
            }

            break;

        default:
            minizip_method = MZ_COMPRESS_METHOD_ZSTD;
            minizip_level  = MZ_COMPRESS_LEVEL_DEFAULT;
            break;
    }

    return {minizip_method, minizip_level};
}

/// Open / create a new file in a zip archive
static inline int open_new_file_in_zip(
    zipFile zip_file,
    const std::filesystem::path& file_path,
    const core::crypto::secure_string& password,
    const int method,
    const int level)
{
    zip_fileinfo zip_file_info;
    const char* const key = password.empty() ? nullptr : password.c_str();

    return zipOpenNewFileInZip5(
        zip_file, // zipFile file
        file_path.c_str(), // const char *filename : the filename in zip
        &zip_file_info, // const zip_fileinfo *zipfi: contain supplemental information
        nullptr, // (UNUSED) const void *extrafield_local: buffer to store the local header extra field data
        0, // (UNUSED) uint16_t size_extrafield_local: size of extrafield_local buffer
        nullptr, // const void *extrafield_global: buffer to store the global header extra field data
        0, // uint16_t size_extrafield_global: size of extrafield_local buffer
        nullptr, // const char *comment: buffer for comment string
        method, // int method: contain the compression method
        level, // int level: contain the level of compression
        0, // int raw: use 0 to disable
        0, // (UNUSED) int windowBits: use default value
        0, // (UNUSED) int memLevel: use default value
        0, // (UNUSED) int strategy: use default value
        key, // const char *password: NULL means no encryption
        0, // (UNUSED) unsigned long crc_for_crypting,
        0, // unsigned long version_madeby
        0, // uint16_t flag_base: use default value
        1 // int zip64: use 0 to disable
        );
}

// ------------------------------------------------------------------------------

/// Implementation class
class ArchiveWriterImpl final : public ArchiveWriter
{
public:
    SIGHT_DECLARE_CLASS(ArchiveWriterImpl, ArchiveWriter)
    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveWriterImpl()                                = delete;
    ArchiveWriterImpl(const ArchiveWriter&)            = delete;
    ArchiveWriterImpl(ArchiveWriter&&)                 = delete;
    ArchiveWriterImpl& operator=(const ArchiveWriter&) = delete;
    ArchiveWriterImpl& operator=(ArchiveWriter&&)      = delete;

    /// Constructor. It opens the archive and creates all resources needed to access it.
    /// @param archive_path path of the archive file. The file will be kept opened as long as the instance lives.
    ArchiveWriterImpl(const std::filesystem::path& archive_path) :
        m_archive_path(archive_path)
    {
        if(std::filesystem::exists(archive_path))
        {
            m_zip_file = zipOpen(archive_path.string().c_str(), APPEND_STATUS_ADDINZIP);
        }
        else
        {
            m_zip_file = zipOpen(archive_path.string().c_str(), APPEND_STATUS_CREATE);
        }

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write("Archive '" + archive_path.string() + "' cannot be opened."),
            m_zip_file == nullptr);
    }

    /// Destructor
    ~ArchiveWriterImpl() override
    {
        if(m_zip_file != nullptr)
        {
            // Close the the zip archive.
            // Last parameter is the global comment which is unused for now.
            const int result = zipClose(m_zip_file, nullptr);

            SIGHT_THROW_EXCEPTION_IF(
                exception::Write("Archive '" + m_archive_path.string() + "' cannot be closed."),
                result != UNZ_OK);

            m_zip_file = nullptr;
        }

        // Remove cached element
        cache_clean(m_archive_path);
    }

    /// Returns a std::ostream to read an archived file
    /// @param file_path path of the file inside the archive.
    /// @param password the password needed to encrypt the file.
    /// @param method the compression algorithm to use.
    /// @param level the compression level to use.
    std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& file_path,
        const core::crypto::secure_string& password = "",
        const Method method                         = Method::ZSTD,
        const Level level                           = Level::DEFAULT) override
    {
        // Boost iostreams Sink. It allows easy ostream implementation
        class ZipSink
        {
        public:
            typedef char char_type;
            typedef boost::iostreams::sink_tag category;

            /// Delete default constructor
            ZipSink() = delete;

            // Boost iostreams Sink do not like constructor with more than one parameter
            // So, we use a sub struct to hold parameters and class attributes.
            struct Parameters
            {
                const ArchiveWriterImpl::sptr m_archive;
                const std::filesystem::path m_file_path;
                const core::crypto::secure_string m_password;
                const int m_method;
                const int m_level;

                Parameters(
                    const ArchiveWriterImpl::sptr& archive,
                    const std::filesystem::path& file_path,
                    const core::crypto::secure_string& password,
                    const int m_method,
                    const int m_level) :
                    m_archive(archive),
                    m_file_path(file_path),
                    m_password(password),
                    m_method(m_method),
                    m_level(m_level)
                {
                }
            };

            ZipSink(const std::shared_ptr<const Parameters>& parameters) :
                m_attributes(parameters),
                m_file_keeper(std::make_shared<const ZipFileKeeper>(m_attributes)),
                m_lock_guard(std::make_shared<std::lock_guard<std::mutex> >(m_attributes->m_archive->m_operation_mutex))
            {
            }

            // ------------------------------------------------------------------------------

            std::streamsize write(const char* buffer, std::streamsize size)
            {
                // Write the data to the archived file
                const int result = zipWriteInFileInZip(
                    m_attributes->m_archive->m_zip_file,
                    buffer,
                    static_cast<std::uint32_t>(size));

                SIGHT_THROW_EXCEPTION_IF(
                    exception::Write(
                        "Cannot write file '"
                        + m_attributes->m_file_path.string()
                        + "' in archive '"
                        + m_attributes->m_archive->m_archive_path.string()
                        + "'."),
                    result != ZIP_OK);

                return size;
            }

        private:
            // Used to create and destroy minizip file handle
            struct ZipFileKeeper
            {
                // Store constructor parameters
                const std::shared_ptr<const Parameters> m_attributes;

                ZipFileKeeper(const std::shared_ptr<const Parameters>& parameters) :
                    m_attributes(parameters)
                {
                    // Open / create the new file inside the archive
                    const int result = open_new_file_in_zip(
                        m_attributes->m_archive->m_zip_file,
                        m_attributes->m_file_path,
                        m_attributes->m_password,
                        m_attributes->m_method,
                        m_attributes->m_level
                        );

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Write(
                            "Cannot open file '"
                            + m_attributes->m_file_path.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archive_path.string()
                            + "'."),
                        result != MZ_OK);
                }

                ~ZipFileKeeper()
                {
                    // Do not forget to clean file handle in zip
                    const int result = zipCloseFileInZip(m_attributes->m_archive->m_zip_file);

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Write(
                            "Cannot close file '"
                            + m_attributes->m_file_path.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archive_path.string()
                            + "'."),
                        result != MZ_OK);
                }
            };

            // Store constructor parameters
            const std::shared_ptr<const Parameters> m_attributes;

            // Used to create and destroy minizip file handle
            const std::shared_ptr<const ZipFileKeeper> m_file_keeper;

            // Locks the archive mutex so nobody could open another file.
            const std::shared_ptr<const std::lock_guard<std::mutex> > m_lock_guard;
        };

        // Translate to minizip dialect
        auto [minizip_method, minizip_level] = to_minizip_parameter(method, level);

        // ZipSink parameters
        auto parameters = std::make_shared<const ZipSink::Parameters>(
            ArchiveWriterImpl::dynamicCast(ArchiveWriterImpl::shared(m_archive_path)),
            file_path,
            password,
            minizip_method,
            minizip_level);

        // Creating a ZipSink also lock the archive mutex.
        // Due to its design, minizip only allows one archive with the same path with one file operation.
        return std::make_unique<boost::iostreams::stream<ZipSink> >(parameters);
    }

private:
    /// Internal class
    friend class ZipSink;

    /// Path of the archive file
    std::filesystem::path m_archive_path;

    /// Archive context handle
    zipFile m_zip_file{nullptr};

    /// Mutex to prevent multiple read/write operation on same file
    std::mutex m_operation_mutex;
};

// ------------------------------------------------------------------------------

ArchiveWriter::sptr ArchiveWriter::shared(const std::filesystem::path& archive_path)
{
    // First try the cache
    ArchiveWriter::sptr new_archive;
    Archive::sptr cached_archive = cache_find(archive_path);

    if(cached_archive)
    {
        // Try to cast in the correct type
        new_archive = ArchiveWriter::dynamicCast(cached_archive);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write("Archive '" + archive_path.string() + "' is already opened in READ mode."),
            !new_archive);
    }

    // If null create a new one
    if(!new_archive)
    {
        // This is a trick to have access to a protected constructor and still allowed to use std::make_shared
        new_archive = std::make_shared<ArchiveWriterImpl>(archive_path);

        // Store the created archive for later use
        cache_store(archive_path, new_archive);
    }

    return new_archive;
}

} // namespace sight::io::zip
