sight_add_target( module_filter_image TYPE MODULE )

target_link_libraries(module_filter_image PUBLIC 
                      data
                      service
                      filter_image
                      ui_base
                      ui_history
)
