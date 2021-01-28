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

#include "arStructuralPatch/data/Camera/V1ToV2.hpp"
#include "arStructuralPatch/data/Camera/V2ToV1.hpp"
#include "arStructuralPatch/data/Camera/V2ToV3.hpp"
#include "arStructuralPatch/data/Camera/V3ToV2.hpp"
#include "arStructuralPatch/data/CameraSeries/V2ToV3.hpp"
#include "arStructuralPatch/data/CameraSeries/V3ToV2.hpp"

#include <io/atoms/patch/StructuralPatchDB.hpp>

namespace arStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {
        io::atoms::patch::StructuralPatchDB::sptr structuralPatches = io::atoms::patch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(::arStructuralPatchdata::Camera::V1ToV2::New());
        structuralPatches->registerPatch(::arStructuralPatchdata::Camera::V2ToV1::New());
        structuralPatches->registerPatch(::arStructuralPatchdata::Camera::V2ToV3::New());
        structuralPatches->registerPatch(::arStructuralPatchdata::Camera::V3ToV2::New());
        structuralPatches->registerPatch(::arStructuralPatchdata::CameraSeries::V2ToV3::New());
        structuralPatches->registerPatch(::arStructuralPatchdata::CameraSeries::V3ToV2::New());
    }

    static runner r;
};
runner runner::r;

} // namespace arStructuralPatch
