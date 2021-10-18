sight_add_target( filter_imageTest TYPE TEST )

target_link_libraries(filter_imageTest PUBLIC 
                      utestData
                      data
                      filter_image
)
