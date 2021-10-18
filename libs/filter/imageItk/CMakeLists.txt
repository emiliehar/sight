sight_add_target( filter_imageItk TYPE LIBRARY PCH pchServicesOmp )

find_package(ITK QUIET COMPONENTS ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT REQUIRED)
target_link_libraries(filter_imageItk PUBLIC ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT)

if(MSVC)
    find_package(ITK QUIET COMPONENTS ITKSmoothing ITKRegistrationMethodsv4 REQUIRED)
    target_link_libraries(filter_imageItk PUBLIC ITKSmoothing ITKRegistrationMethodsv4)
endif()

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(filter_imageItk PRIVATE OpenMP::OpenMP_CXX)
endif()

find_package(glm QUIET REQUIRED)
target_include_directories(filter_imageItk SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})

target_link_libraries(filter_imageItk PUBLIC data geometry_data io_itk filter_image)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
