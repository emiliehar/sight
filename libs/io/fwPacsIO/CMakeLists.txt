fwLoadProperties()

find_package(DCMTK QUIET REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIRS} )
fwForwardLink( ${DCMTK_LIBRARIES} )

set(SIGHT_DCMTK_CONFIG "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

if(UNIX)
    if(SIGHT_SYSTEM_LIB)
        file(COPY "/etc/dcmtk/storescp.cfg" DESTINATION "${SIGHT_DCMTK_CONFIG}")
    elseif(DCMTK_DIR)
        file(COPY "${DCMTK_DIR}/../../../etc/dcmtk/storescp.cfg" DESTINATION "${SIGHT_DCMTK_CONFIG}")
    elseif(DCMTK_LIB_DIRS)
        file(COPY "${DCMTK_LIB_DIRS}/../etc/dcmtk/storescp.cfg" DESTINATION "${SIGHT_DCMTK_CONFIG}")
    endif()
elseif(DCMTK_DIR)
    file(COPY "${DCMTK_DIR}/../etc/dcmtk/storescp.cfg" DESTINATION "${SIGHT_DCMTK_CONFIG}")
elseif(DCMTK_LIB_DIRS)
    file(COPY "${DCMTK_LIB_DIRS}/../etc/dcmtk/storescp.cfg" DESTINATION "${SIGHT_DCMTK_CONFIG}")
endif()

unset(SIGHT_DCMTK_CONFIG)
