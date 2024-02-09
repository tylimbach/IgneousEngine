cmake_minimum_required(VERSION 3.4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(APPLE)
    set(ENGINE_LIB_SUFFIX ".a")
else()
    set(ENGINE_LIB_SUFFIX ".lib")
endif()

set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/engine")

message("loading engine lib at ${CMAKE_BINARY_DIR}")

project(app)

set(spdlog_DIR "${PROJECT_SOURCE_DIR}/../engine/vendor/spdlog/build")
if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_executable(app src/application.cpp ${PROJECT_SOURCE_DIR} "src/lua_scripts.h")

find_library(ENGINE_LIB IgneousEngine)
add_dependencies(app IgneousEngine)
target_link_libraries(app IgneousEngine spdlog::spdlog)
target_include_directories(app PRIVATE ${ENGINE_DIR} )

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE )
 # target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE )
 # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

