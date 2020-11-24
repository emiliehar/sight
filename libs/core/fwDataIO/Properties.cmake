
set( NAME fwDataIO )
set( VERSION 0.2 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataTools
    fwTools
    fwJobs
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    SIGHT_CONAN_ZLIB
)
set( WARNINGS_AS_ERRORS ON)
