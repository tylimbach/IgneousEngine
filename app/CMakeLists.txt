cmake_minimum_required(VERSION 3.4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENGINE_LIB "${CMAKE_BINARY_DIR}/IgneousEngine.lib")
set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/engine")

message("loading engine lib at ${CMAKE_BINARY_DIR}")

project(app)

set(spdlog_DIR "${PROJECT_SOURCE_DIR}/../engine/vendor/spdlog/build")
if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_executable(app src/application.cpp ${PROJECT_SOURCE_DIR})

find_library(ENGINE_LIB IgneousEngine)
target_link_libraries(app ${ENGINE_LIB} spdlog::spdlog)
target_include_directories(app PRIVATE ${ENGINE_DIR} )

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

