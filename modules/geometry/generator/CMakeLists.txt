sight_add_target( module_geometry_generator TYPE MODULE )

target_link_libraries(module_geometry_generator PUBLIC 
                      core
                      data
                      ui_base
                      service
                      geometry_data
)
