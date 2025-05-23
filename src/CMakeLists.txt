find_package(TOSUNAPI REQUIRED)

add_library(tosunapi SHARED tosunapi.cc)
target_include_directories(tosunapi PUBLIC
        ${TOSUNAPI_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tosunapi PUBLIC ${TOSUNAPI_LIBRARIES})
