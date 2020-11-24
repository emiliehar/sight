set( NAME videoOpenCV )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        fwCore
        fwData
        fwRuntime
        fwServices
        fwTools
        fwIO
        arData
        fwCom
        fwGui
        arServices
        arPreferences
)
set( REQUIREMENTS  )
set( CONAN_DEPS
    SIGHT_CONAN_BOOST
    SIGHT_CONAN_OPENCV
    SIGHT_CONAN_FFMPEG
)
