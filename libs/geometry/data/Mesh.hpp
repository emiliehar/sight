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

#include "geometry/data/config.hpp"
#include "geometry/data/Vector.hxx"

#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>

#include <core/base.hpp>

namespace sight::geometry::data
{

/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class Mesh
{
public:

    typedef std::map<Point, sight::data::Mesh::PointId> PointsMapType;
    typedef std::array<double, 3> PositionType;
    typedef std::array<size_t, 3> OrganizationType;

    /**
     * @brief Initialize 'rand' seed
     */
    DATA_API static void initRand();

    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  mesh data::Mesh structure to fill with cell normals.
     */
    DATA_API static void generateCellNormals(sight::data::Mesh::sptr mesh);

    /**
     * @brief Generate point normals for the mesh.
     *
     * @param[out]  mesh data::Mesh structure to fill with cell normals.
     */
    DATA_API static void generatePointNormals(sight::data::Mesh::sptr mesh);

    /**
     * @brief Shake Array of Normals.
     *
     * Array must have 3 float components, 1 dimension. Otherwise, shakeNormals
     * will do nothing.
     *
     * @param[out]  array mesh's array containing normals.
     * @deprecated This method will be removed because mesh arrays can no longer be accessed.
     */
    [[deprecated("will be removed in sight 22.0")]] DATA_API static void shakeNormals(
        sight::data::Array::sptr array
    );

    /**
     * @brief Shake point Normals.
     *
     * @param[out]  mesh data::Mesh structure to shake normals.
     */
    DATA_API static void shakePointNormals(sight::data::Mesh::sptr mesh);

    /**
     * @brief Shake cell Normals.
     *
     * @param[out]  mesh data::Mesh structure to shake normals.
     */
    DATA_API static void shakeCellNormals(sight::data::Mesh::sptr mesh);

    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  mesh data::Mesh structure to shake.
     */
    DATA_API static void shakePoint(sight::data::Mesh::sptr mesh);

    /**
     * @brief Colorize mesh (vertex point color).
     *
     * @param[in]  mesh data::Mesh mesh structure to colorize.
     */
    DATA_API static void colorizeMeshPoints(sight::data::Mesh::sptr mesh);

    /**
     * @brief Colorize mesh (cell color).
     *
     * @param[in]  mesh data::Mesh mesh structure to colorize.
     */
    DATA_API static void colorizeMeshCells(sight::data::Mesh::sptr mesh);

    /**
     * @brief
     *
     * @param[in]  mesh data::Mesh mesh structure to find cell type.
     * @param[in]  cell CellTypes to find in mesh.
     */
    DATA_API static bool hasUniqueCellType(sight::data::Mesh::csptr mesh, sight::data::Mesh::CellTypes cell);

    /**
     * @brief
     *
     * @param[in]  mesh data::Mesh mesh structure to find cell type.
     * @param[in]  cell CellType to find in mesh (POINT, EDGE, TRAINGLE, ...).
     */
    DATA_API static bool hasUniqueCellType(sight::data::Mesh::csptr mesh, sight::data::Mesh::CellType cell);

    /// Apply a transformation 4x4 from an input mesh to an output mesh
    DATA_API static void transform(
        sight::data::Mesh::csptr inMesh,
        sight::data::Mesh::sptr outMesh,
        sight::data::Matrix4::csptr t
    );

    /// Apply a transformation 4x4 on a mesh
    DATA_API static void transform(sight::data::Mesh::sptr mesh, sight::data::Matrix4::csptr t);

    /**
     * @brief Colorize the mesh points with the given color
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    DATA_API static void colorizeMeshPoints(
        const sight::data::Mesh::sptr& mesh,
        const std::uint8_t colorR,
        const std::uint8_t colorG,
        const std::uint8_t colorB,
        const std::uint8_t colorA = 255
    );

    /**
     * @brief Colorize the mesh points associated to the triangle in _vectorNumTriangle
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    DATA_API static void colorizeMeshPoints(
        const sight::data::Mesh::sptr& _mesh,
        const std::vector<size_t>& _vectorNumTriangle,
        const std::uint8_t _colorR,
        const std::uint8_t colorG,
        const std::uint8_t colorB,
        const std::uint8_t _colorA = 255
    );

    /**
     * @brief Colorize the mesh cells with the given rgb color
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    DATA_API static void colorizeMeshCells(
        const sight::data::Mesh::sptr& mesh,
        const std::uint8_t colorR,
        const std::uint8_t colorG,
        const std::uint8_t colorB,
        const std::uint8_t _colorA = 255
    );
    /**
     * @brief Colorize the cells in vectorNumTriangle
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    DATA_API static void colorizeMeshCells(
        const sight::data::Mesh::sptr& mesh,
        const std::vector<size_t>& triangleIndexVector,
        const std::uint8_t colorR,
        const std::uint8_t colorG,
        const std::uint8_t colorB,
        const std::uint8_t _colorA = 255
    );

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    DATA_API static bool isClosed(const sight::data::Mesh::csptr& mesh);
};

} // namespace sight::geometry::data
