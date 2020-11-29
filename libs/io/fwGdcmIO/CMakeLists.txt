fwLoadProperties()

find_package(Boost QUIET COMPONENTS locale REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE boost_locale PUBLIC gdcmMSFF)
