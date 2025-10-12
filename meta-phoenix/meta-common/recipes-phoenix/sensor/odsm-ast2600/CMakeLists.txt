project (odsm)

cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set (CMAKE_SKIP_BUILD_RPATH  TRUE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} SensorAPIs)
link_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_library(ptecsensorraw SHARED
    raw_reading.cpp
    raw_reading_info.cpp
)

set_target_properties(ptecsensorraw PROPERTIES SOVERSION 0.0.1)
target_link_libraries (ptecsensorraw ptecsensorapi)

set (SERVICE_FILES
     ${PROJECT_SOURCE_DIR}/odsm.service)
install (FILES ${SERVICE_FILES} DESTINATION /lib/systemd/system/)

