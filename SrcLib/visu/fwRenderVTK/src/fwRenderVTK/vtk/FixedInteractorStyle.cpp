/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderVTK/vtk/FixedInteractorStyle.hpp"

#include <fwCore/base.hpp>

#include <vtkCommand.h>
#include <vtkObjectFactory.h>

namespace fwRenderVTK
{
namespace vtk
{

vtkStandardNewMacro(FixedInteractorStyle);

//------------------------------------------------------------------------------

FixedInteractorStyle::FixedInteractorStyle() :
    vtkInteractorStyleUser()
{
}

//------------------------------------------------------------------------------

FixedInteractorStyle::~FixedInteractorStyle()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnChar()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnLeftButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnRightButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnMiddleButtonDown()
{
}

//------------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseMove()
{
}

//----------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseWheelBackward()
{
}

//----------------------------------------------------------------------------

void FixedInteractorStyle::OnMouseWheelForward()
{
}

} // namespace vtk
} // namespace fwRenderVTK
