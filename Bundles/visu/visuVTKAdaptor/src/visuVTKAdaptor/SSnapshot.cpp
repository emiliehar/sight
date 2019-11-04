/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/SSnapshot.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>
#include <vtkBMPWriter.h>
#include <vtkImageData.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SSnapshot)

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType SSnapshot::s_SNAP_SLOT        = "snap";
const ::fwCom::Slots::SlotKeyType SSnapshot::s_SNAPTOIMAGE_SLOT = "snapToImage";

SSnapshot::SSnapshot() noexcept
{
    newSlot(s_SNAP_SLOT, &SSnapshot::snap, this);
    newSlot(s_SNAPTOIMAGE_SLOT, &SSnapshot::snapToImage, this);
}

//------------------------------------------------------------------------------

SSnapshot::~SSnapshot() noexcept
{
}

//------------------------------------------------------------------------------

void SSnapshot::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SSnapshot::starting()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SSnapshot::updating()
{
}

//------------------------------------------------------------------------------

void SSnapshot::stopping()
{
}

//------------------------------------------------------------------------------

void SSnapshot::snapToImage()
{
    if ( !m_imageUid.empty() )
    {
        ::fwData::Image::sptr imageToSnap = ::fwData::Image::New();

        vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
#if VTK_MAJOR_VERSION > 7
        snapper->SetScale( 1 );
#else
        snapper->SetMagnification( 1 );
#endif
        snapper->SetInput( this->getRenderer()->GetRenderWindow() );
        snapper->Update();

        vtkImageData* vtkImage = snapper->GetOutput();
        ::fwVtkIO::fromVTKImage(vtkImage, imageToSnap);

        this->setOutput("image", imageToSnap);

        snapper->Delete();
    }
}

//------------------------------------------------------------------------------

void SSnapshot::snap(std::string filePath)
{
    SLM_ASSERT("filePath is empty", !filePath.empty());
    namespace fs = ::boost::filesystem;
    fs::path pathImageSnap(filePath);

    std::string ext = ".jpg";
    ext = pathImageSnap.extension().string();
    vtkImageWriter* writer = 0;

    if( ext == ".jpg" || ext == ".jpeg" )
    {
        writer = vtkJPEGWriter::New();
    }
    else if ( ext == ".bmp" )
    {
        writer = vtkBMPWriter::New();
    }
    else if ( ext == ".tiff" )
    {
        writer = vtkTIFFWriter::New();
    }
    else if ( ext == ".png" )
    {
        writer = vtkPNGWriter::New();
    }
    else
    {
        SLM_FATAL("Error: Format is not supported.");
    }

    vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
#if VTK_MAJOR_VERSION > 7
    snapper->SetScale( 1 );
#else
    snapper->SetMagnification( 1 );
#endif
    snapper->SetInput( this->getRenderer()->GetRenderWindow() );

    writer->SetInputConnection( snapper->GetOutputPort() );
    writer->SetFileName( pathImageSnap.string().c_str() );
    writer->Write();

    snapper->Delete();
    writer->Delete();
}

} //namespace visuVTKAdaptor
