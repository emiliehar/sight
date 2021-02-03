/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "reflection/ObjectTest.hpp"

#define CAMP_COMPILATION
#include <fwData/Image.hpp>

#include <fwData/reflection/getObject.hpp>

#include <camp/class.hpp>
#include <camp/userobject.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::reflection::ut::ObjectTest );

namespace fwData
{
namespace reflection
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void ObjectTest::setUp()
{
}

//-----------------------------------------------------------------------------

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ObjectTest::callFunctionTest()
{
    ::fwData::Image::sptr imgage = ::fwData::Image::New();
    ::camp::UserObject campObj(imgage.get());
    const ::camp::Class& metaClass = campObj.getClass();
    CPPUNIT_ASSERT(metaClass.hasFunction("classname"));
    CPPUNIT_ASSERT(metaClass.hasFunction("is_a"));

    CPPUNIT_ASSERT(campObj.call("classname").to<std::string>() == "::fwData::Image");
    CPPUNIT_ASSERT(campObj.call("is_a", ::camp::Args("::fwData::Object")).to<bool>());
    CPPUNIT_ASSERT(campObj.call("is_a", ::camp::Args("::fwData::Image")).to<bool>());
    CPPUNIT_ASSERT(!campObj.call("is_a", ::camp::Args("::fwData::Float")).to<bool>());

}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace reflection
} // namespace fwData
