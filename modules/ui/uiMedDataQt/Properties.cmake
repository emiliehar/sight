
set( NAME uiMedDataQt )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    fwActivities
    fwCom
    fwCore
    fwData
    fwDataCamp
    fwDataTools
    fwGui
    fwGuiQt
    fwMedData
    fwMedDataTools
    fwRuntime
    fwServices
    fwTools
    fwDataTools
    fwIO
    )
set( REQUIREMENTS
    fwMedData
    servicesReg
    uiImageQt
    ctrlSelection
    )
set( CONAN_DEPS
    SIGHT_CONAN_BOOST
    SIGHT_CONAN_QT
)