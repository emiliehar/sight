set(NAME hybridMarkerTracker)
set(VERSION 0.1)
set( TYPE MODULE )
set( DEPENDENCIES
        fwRuntime
        fwCore
        fwData
        fwDataTools
        fwServices
        fwTools
        fwMath
        fwCom
        arData
        fwMedData
        arServices
        cvIO
)
set( REQUIREMENTS fwData
        servicesReg
        arData
)
set( CONAN_DEPS
        SIGHT_CONAN_OPENCV
        SIGHT_CONAN_IPPE
        SIGHT_CONAN_HYBRID-MARKER-TRACKER
)