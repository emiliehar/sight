/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <core/runtime/EConfigurationElement.hpp>

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::filter::dicom
{

namespace ut
{

/**
 * @brief Test ImagePositionPatientSorter class
 */
class ImagePositionPatientSorterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ImagePositionPatientSorterTest);
CPPUNIT_TEST(simpleApplication);
CPPUNIT_TEST(applyFilterOnMultipleVolumeImage);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    /// Apply the patch and verify that the DicomSeries has been correctly modified
    void simpleApplication();

    /// Apply the filter on an unsupported multiple volume image
    void applyFilterOnMultipleVolumeImage();
};

} // namespace ut

} // namespace sight::filter::dicom
