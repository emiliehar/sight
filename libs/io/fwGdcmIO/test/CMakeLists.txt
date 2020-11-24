fwLoadProperties()

find_package(GDCM QUIET REQUIRED)

fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

