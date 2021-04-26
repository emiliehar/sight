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

#include "PasswordKeeper.hpp"

#include <core/crypto/AES256.hpp>
#include <core/crypto/SHA256.hpp>

#include <iomanip>
#include <mutex>

namespace sight::io::session
{

// Protects password concurent access
static std::mutex s_password_mutex;

// The static global password. Stored encrypted to make direct memory reading a bit harder
static core::crypto::secure_string s_password;

// This generate the hash used to encrypt the global password
inline static core::crypto::secure_string computeGlobalPasswordKey()
{
    return SIGHT_PSEUDO_RANDOM_HASH("");
}

class PasswordKeeperImpl final : public PasswordKeeper
{
public:
    SIGHT_DECLARE_CLASS(PasswordKeeperImpl, PasswordKeeper)
    /// Delete default copy constructors and assignment operators
    PasswordKeeperImpl(const PasswordKeeperImpl&)            = delete;
    PasswordKeeperImpl(PasswordKeeperImpl&&)                 = delete;
    PasswordKeeperImpl& operator=(const PasswordKeeperImpl&) = delete;
    PasswordKeeperImpl& operator=(PasswordKeeperImpl&&)      = delete;

    /// Default constructor
    PasswordKeeperImpl() = default;

    /// Default destructor
    ~PasswordKeeperImpl() override = default;

    // ------------------------------------------------------------------------------

    core::crypto::secure_string getPasswordHash() const override
    {
        return core::crypto::hash(this->getPassword());
    }

    // ------------------------------------------------------------------------------

    core::crypto::secure_string getPassword() const override
    {
        return core::crypto::decrypt(m_password, computePasswordKey());
    }

    // ------------------------------------------------------------------------------

    void setPassword(const core::crypto::secure_string& password) override
    {
        m_password = core::crypto::encrypt(password, computePasswordKey());
    }

    // ------------------------------------------------------------------------------

    bool checkPassword(const core::crypto::secure_string& password) const override
    {
        return core::crypto::decrypt(m_password, computePasswordKey()) == password;
    }

private:
    /// Generate a pseudo random password key to store the password obfuscated
    inline core::crypto::secure_string computePasswordKey() const
    {
        return SIGHT_PSEUDO_RANDOM_HASH(std::to_string(reinterpret_cast<std::intptr_t>(this)));
    }

    /// Holds the local password. It is stored as an AES encrypted string
    core::crypto::secure_string m_password;
};

/// Factory
PasswordKeeper::sptr PasswordKeeper::shared()
{
    return std::make_shared<PasswordKeeperImpl>();
}

// ------------------------------------------------------------------------------

PasswordKeeper::uptr PasswordKeeper::unique()
{
    return std::make_unique<PasswordKeeperImpl>();
}

// ------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::getGlobalPasswordHash()
{
    return core::crypto::hash(PasswordKeeper::getGlobalPassword());
}

// ------------------------------------------------------------------------------

core::crypto::secure_string PasswordKeeper::getGlobalPassword()
{
    std::lock_guard<std::mutex> guard(s_password_mutex);

    return core::crypto::decrypt(s_password, computeGlobalPasswordKey());
}

// ------------------------------------------------------------------------------

void PasswordKeeper::setGlobalPassword(const core::crypto::secure_string& password)
{
    std::lock_guard<std::mutex> guard(s_password_mutex);
    s_password = core::crypto::encrypt(password, computeGlobalPasswordKey());
}

// ------------------------------------------------------------------------------

bool PasswordKeeper::checkGlobalPassword(const core::crypto::secure_string& password)
{
    std::lock_guard<std::mutex> guard(s_password_mutex);

    return core::crypto::decrypt(s_password, computeGlobalPasswordKey()) == password;
}

} // namespace sight::io::session
