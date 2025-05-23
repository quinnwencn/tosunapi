cmake_minimum_required(VERSION 3.15)
project(tosun_can_api)

set(CMAKE_CXX_STANDARD 20)

if (LINUX)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
endif ()

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cc src/tosunapi.cc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE tosunapi)
