fwLoadProperties()

find_package(VTK QUIET REQUIRED)
find_package(GDCM QUIET REQUIRED)

fwForwardInclude(
    ${GDCM_INCLUDE_DIRS}
)

fwLink(
    ${GDCM_LIBRARIES}
    ${VTK_LIBRARIES}

    vtkgdcm
    gdcmMSFF
    gdcmDSED
    gdcmCommon
)
