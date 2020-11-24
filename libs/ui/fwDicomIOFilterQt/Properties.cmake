set( NAME fwDicomIOFilterQt )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwTools
    fwMedData
    fwDicomIOFilter
    fwGui
    fwGuiQt
)
set( REQUIREMENTS )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    SIGHT_CONAN_DCMTK
    SIGHT_CONAN_QT
)
