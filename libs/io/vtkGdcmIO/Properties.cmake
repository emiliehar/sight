set( NAME vtkGdcmIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataIO
    fwMedData
    fwMemory
    fwTools
    fwVtkIO
    fwJobs
)
set( REQUIREMENTS )
set( CONAN_DEPS
    SIGHT_CONAN_VTK
    SIGHT_CONAN_GDCM
)