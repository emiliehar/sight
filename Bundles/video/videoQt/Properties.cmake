
set( NAME videoQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwDataTools
        fwGuiQt
        arServices
        arData
        arPreferences
        fwVideoQt
)
set( REQUIREMENTS  )
set( PLUGINS
    mediaservice
)
set( CONAN_DEPS
    ${CONAN_QT}
)