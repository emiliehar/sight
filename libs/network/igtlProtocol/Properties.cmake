set( NAME igtlProtocol )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES   
    fwCore
    fwRuntime
    fwData
    fwServices
    fwTools
    fwZip
    fwAtoms
    fwAtomConversion
    fwAtomsBoostIO
    fwDataTools
)
set( REQUIREMENTS  )
set( CONAN_DEPS
    SIGHT_CONAN_LIBARCHIVE
    SIGHT_CONAN_OPENIGTLINK
)