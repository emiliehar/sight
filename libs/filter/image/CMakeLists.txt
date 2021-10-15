sight_add_target( filter_image TYPE LIBRARY PCH pchServicesOmp )

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(filter_image PRIVATE OpenMP::OpenMP_CXX)
endif()

find_package(glm QUIET REQUIRED)
target_include_directories(filter_image SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})

target_link_libraries(filter_image PUBLIC data geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
