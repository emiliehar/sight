fwLoadProperties()

find_package(Boost QUIET COMPONENTS date_time log log_setup REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME} PUBLIC boost_date_time boost_log boost_log_setup)
