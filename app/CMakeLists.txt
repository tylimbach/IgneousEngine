cmake_minimum_required(VERSION 3.4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENGINE_LIB "${CMAKE_BINARY_DIR}/BabyVulkanEngine.lib")
set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/engine")

message("loading engine lib at ${CMAKE_BINARY_DIR}")

project(app)

add_executable(app src/application.cpp)

find_library(ENGINE_LIB BabyVulkanEngine)
target_link_libraries(app ${ENGINE_LIB})
target_include_directories(app PRIVATE ${ENGINE_DIR})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

