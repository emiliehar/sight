set( NAME Tuto05Mesher )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_qt                   # Start the module, load qt implementation of module_ui_base

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    style

    # Reader
    module_io_vtk

    # Services
    uiIO
    visuBasic   # contains a visualization service of mesh.
    opVTKMesh       # provides services to generate a mesh from an image.
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto05Mesher_AppCfg
) # Main application's configuration to launch
