
set( NAME PoCStereo )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioData
    ioAtoms
    io
    ioVTK
    uiIO
    fwlauncher
    appXml
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    uiVisuOgre
    material
    media
    preferences
    ogreConfig
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoCStereo)

