set( NAME imageFilterOp )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwData
    fwDataTools
    fwItkIO
    fwTools
)
set( REQUIREMENTS)
set( CONAN_DEPS
    SIGHT_CONAN_ITK
    SIGHT_CONAN_GLM
)
set( USE_PCH_FROM_TARGET pchServicesOmp )
