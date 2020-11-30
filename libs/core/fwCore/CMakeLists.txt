fwLoadProperties()

find_package(Boost QUIET COMPONENTS date_time log log_setup REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::date_time Boost::log Boost::log_setup)
