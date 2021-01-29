set( NAME Tuto03DataService )
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

    # Reader
    module_io_vtk

    # Services
    uiIO                    # Contains services to show dialogs for reader/writer selection
    visuBasic
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto03DataService_AppCfg
) # Main application's configuration to launch
