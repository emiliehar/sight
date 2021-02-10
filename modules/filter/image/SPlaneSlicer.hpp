/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <data/tools/helper/MedicalImage.hpp>

#include <services/IOperator.hpp>

#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

namespace sight::modules::filter::image
{

/**
 * @brief Operator computing a 2D slice from a 3D image.
 *
 * @section Slots Slots
 * - \b updateSliceType(int, int) : change slice
 * - \b updateDefaultValue() : set the image minimum as the reslicer background value.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::filter::image::SPlaneSlicer" autoConnect="yes">
       <in key="image" uid="..." />
       <in key="imageExtent" uid="..." />
       <in key="axes" uid="..." />
       <inout key="slice" uid="..." />
       <config orientation="axial" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::Image]: Image from which the slice is extracted.
 * - \b imageExtent [sight::data::Image]: Image used to pass the extent.
 * - \b axes [sight::data::TransformationMatrix3D]: Matrix containing the axes defining the reslicing plane.
 *
 * @subsection In-Out In-Out
 * - \b slice [sight::data::Image]: Extracted slice.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : operator configuration
 *    - \b orientation (mandatory, values=axial|sagital|frontal): image orientation, determines the slicing plane axes.
 */

class MODULE_FILTER_IMAGE_CLASS_API SPlaneSlicer : public services::IOperator
{

public:

    fwCoreServiceMacro(SPlaneSlicer, services::IOperator)

    /// Constructor.
    MODULE_FILTER_IMAGE_API SPlaneSlicer() noexcept;

    /// Destructor.
    MODULE_FILTER_IMAGE_API virtual ~SPlaneSlicer() noexcept;

protected:

    MODULE_FILTER_IMAGE_API virtual void configuring() override;
    MODULE_FILTER_IMAGE_API virtual void starting() override;
    MODULE_FILTER_IMAGE_API virtual void updating() override;
    MODULE_FILTER_IMAGE_API virtual void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     *
     * Connect image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_DEFAULT_VALUE_SLOT
     * Connect imageExtent::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect imageExtent::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect axes::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_FILTER_IMAGE_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Set the extent based on the extentImage and orientation.
    void setReslicerExtent();

    /// Set the plane axes using the input transform.
    void setReslicerAxes();

    /// Modify the matrix parameter to align the plane with the current extent image slice.
    void applySliceTranslation(vtkSmartPointer<vtkMatrix4x4> vtkMat) const;

    /// Slot: called when the extent image orientation.
    void updateSliceOrientation(int from, int to);

    /// Slot: sets the reslicer default value which is the minimum value of the input image.
    void updateDefaultValue();

    /// Slicing orientation. Determines the two axes defining the plane.
    data::tools::helper::MedicalImage::Orientation m_orientation;

    /// Vtk reslicing algorithm.
    vtkSmartPointer<vtkImageReslice> m_reslicer;
};

} //namespace sight::modules::filter::image
