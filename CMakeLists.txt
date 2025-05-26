cmake_minimum_required(VERSION 3.15)
project(tosun_can_api)

set(CMAKE_CXX_STANDARD 20)

if (LINUX)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
endif ()

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cc src/tosunapi.cc)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PRIVATE tosunapi)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/tosunapi
        DESTINATION include/tosunapi
        FILES_MATCHING PATTERN "*.h")
