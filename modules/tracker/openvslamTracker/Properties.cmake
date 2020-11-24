set( NAME openvslamTracker )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        fwCore
        fwCom
        fwData
        fwDataTools
        fwRuntime
        fwServices
        fwThread
        arServices
        fwGui
        arData
        cvIO
        openvslamIO
)
set( REQUIREMENTS )

set( CONAN_DEPS
    SIGHT_CONAN_BOOST
    SIGHT_CONAN_OPENCV
    SIGHT_CONAN_EIGEN
    SIGHT_CONAN_OPENVSLAM
)
