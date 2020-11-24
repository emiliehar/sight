fwLoadProperties()

find_package(DCMTK QUIET REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

find_package(GDCM QUIET REQUIRED)
fwForwardInclude( ${GDCM_INCLUDE_DIRS} )
fwLink(
    gdcmCommon
    gdcmMSFF
)

