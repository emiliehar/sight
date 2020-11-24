# Conan support
file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    SHOW_PROGRESS
    EXPECTED_HASH SHA256=75c92be7d739ab69c3c9a1cd0bf4728cd08da143a18776eb43f8e2af16accace)
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(VERSION 1.31.0 REQUIRED)
conan_add_remote(NAME sight INDEX 1
                 URL https://conan.ircad.fr/artifactory/api/conan/sight)

unset(CONAN_OPTIONS)
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
    set(CONAN_OPTIONS "*:cuda=${CUDAToolkit_VERSION_MAJOR}")
    message(STATUS "CUDA ${CUDAToolkit_VERSION_MAJOR} found on your PC. Cuda Conan packages will be used.\n"
                   "Conan options used: ${CONAN_OPTIONS}")
else()
    set(CONAN_OPTIONS "*:cuda=None")
    message(STATUS "CUDA not found on your PC. Cuda Conan packages will be NOT used.\n"
                   "Conan options used: ${CONAN_OPTIONS}")
endif()

# Not sure why it is our responsability to set this, so CMake can use conan generated FindPackage
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

macro(findConanDeps PROJECT_LIST SIGHT_CONAN_DEPS_LIST)
    unset(RESULT_LIST)

    foreach(PROJECT ${PROJECT_LIST})
        if(${PROJECT}_CONAN_DEPS)
            list(APPEND RESULT_LIST ${${PROJECT}_CONAN_DEPS})
        endif()
    endforeach()

    # Also add the sight-flags package which contains flags.cmake used for sharing C flags across conan packages
    list(APPEND RESULT_LIST SIGHT_CONAN_SIGHT-FLAGS)

    list(REMOVE_DUPLICATES RESULT_LIST)
    set(SIGHT_CONAN_DEPS_LIST ${RESULT_LIST})
    unset(RESULT_LIST)
endmacro()

macro(installConanDeps SIGHT_CONAN_DEPS_LIST)
    if(SIGHT_CONAN_BUILD_ENABLED)
        set(CONAN_BUILD_OPTION "missing")
    else()
        set(CONAN_BUILD_OPTION "never")
    endif()

    # Force use of c++17 as standard
    if(MSVC)
        list(APPEND CONAN_SETTINGS "compiler.cppstd=17")
    else()
        list(APPEND CONAN_SETTINGS "compiler.cppstd=gnu17")
    endif()

    option(SIGHT_CONAN_VERBOSE "Verbose conan output" ON)

    unset(DEFAULT_GENERATORS_DEPS)
    unset(CUSTOM_GENERATORS_DEPS)

    # Try to triage packages
    foreach(CONAN_DEP ${SIGHT_CONAN_DEPS_LIST})
        if(${CONAN_DEP}_GENERATORS)
            list(APPEND CUSTOM_GENERATORS_DEPS ${CONAN_DEP})
        else()
            # Dereference immediatly to have the conan reference 
            list(APPEND DEFAULT_GENERATORS_DEPS ${${CONAN_DEP}})
        endif()
    endforeach()

    # For each package with custom generators
    foreach(CONAN_DEP ${CUSTOM_GENERATORS_DEPS})
        if(SIGHT_CONAN_VERBOSE)
            conan_cmake_run(
                REQUIRES ${${CONAN_DEP}}
                BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
                GENERATORS ${${CONAN_DEP}_GENERATORS}
                OPTIONS ${CONAN_OPTIONS}
                BUILD ${CONAN_BUILD_OPTION}
                SETTINGS ${CONAN_SETTINGS}
            )
        else()
            conan_cmake_run(
                REQUIRES ${${CONAN_DEP}}
                BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS OUTPUT_QUIET
                GENERATORS ${${CONAN_DEP}_GENERATORS}
                OPTIONS ${CONAN_OPTIONS}
                BUILD ${CONAN_BUILD_OPTION}
                SETTINGS ${CONAN_SETTINGS}
            )
        endif()
    endforeach()

    # Install package with default generator
    if(SIGHT_CONAN_VERBOSE)
        conan_cmake_run(
            REQUIRES ${DEFAULT_GENERATORS_DEPS}
            BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
            GENERATORS "cmake;cmake_find_package"
            OPTIONS ${CONAN_OPTIONS}
            BUILD ${CONAN_BUILD_OPTION}
            SETTINGS ${CONAN_SETTINGS}
        )
    else()
        conan_cmake_run(
            REQUIRES ${DEFAULT_GENERATORS_DEPS}
            BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS OUTPUT_QUIET
            GENERATORS "cmake;cmake_find_package"
            OPTIONS ${CONAN_OPTIONS}
            BUILD ${CONAN_BUILD_OPTION}
            SETTINGS ${CONAN_SETTINGS}
        )
    endif()

    unset(DEFAULT_GENERATORS_DEPS)
    unset(CUSTOM_GENERATORS_DEPS)
endmacro()

macro(installConanDepsForSDK)

    foreach(CONAN_DEP ${CONAN_DEPENDENCIES})
        string(REGEX REPLACE "([^\/]*)/.*" "\\1" CONAN_REQUIREMENT ${CONAN_DEP})
        string(TOUPPER ${CONAN_REQUIREMENT} CONAN_REQUIREMENT )
        install(DIRECTORY "${CONAN_${CONAN_REQUIREMENT}_ROOT}/" DESTINATION "."
                USE_SOURCE_PERMISSIONS
                PATTERN "qt.conf" EXCLUDE # We remove the qt.conf from Qt package because it does not match
                                          # our plugin path. Our plugin path is set in WorkerQt.cpp
                )
    endforeach()

endmacro()
