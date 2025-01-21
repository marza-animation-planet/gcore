cmake_minimum_required(VERSION 3.26...3.31)
project(gcore)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if(WIN32 AND BUILD_SHARED_LIBS AND NOT CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(Threads REQUIRED)

file(GLOB gcore_source CONFIGURE_DEPENDS src/lib/*.cpp src/lib/rex/*.cpp)
file(GLOB gcore_headers CONFIGURE_DEPENDS include/gcore/*.h include/half.h)

add_library(gcore ${gcore_source})
target_link_libraries(gcore Threads::Threads)
target_sources(gcore PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
    BASE_DIRS include
    FILES ${gcore_headers}
)

add_executable(readbom src/bin/readbom.cpp)
add_executable(writebom src/bin/writebom.cpp)
target_link_libraries(readbom gcore)
target_link_libraries(writebom gcore)

if(NOT ${BUILD_SHARED_LIBS})
    target_compile_definitions(gcore PUBLIC GCORE_STATIC)
endif()

if(WIN32)
    target_compile_definitions(
        gcore PUBLIC _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES _WIN32_WINNT=0x0A00
    )
else()
    target_link_libraries(gcore ${CMAKE_DL_LIBS})
endif()
set_property(TARGET gcore PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(gcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS gcore readbom writebom EXPORT gcoreTargets FILE_SET public_headers)
# TODO still missing config
install(EXPORT gcoreTargets FILE gcoreTargets.cmake NAMESPACE gcore:: DESTINATION lib/cmake/gcore)
