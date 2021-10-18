sight_add_target( filter_imageItkTest TYPE TEST )

target_link_libraries(filter_imageItkTest PUBLIC 
                      utestData
                      data
                      io_vtk
                      filter_imageItk
)
