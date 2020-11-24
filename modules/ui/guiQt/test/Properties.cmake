
set( NAME guiQtTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwTest
    fwCore
    fwData
    fwRuntime
    fwServices
)
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
)

set( CONAN_DEPS
    SIGHT_CONAN_QT
)
