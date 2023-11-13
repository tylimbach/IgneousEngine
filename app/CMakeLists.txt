cmake_minimum_required(VERSION 3.4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENGINE_LIB "${CMAKE_BINARY_DIR}/BabyVulkanEngine.lib")
set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/engine")

message("loading engine lib at ${CMAKE_BINARY_DIR}")


#temporary

if (DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
  set(Vulkan_FOUND "True")
else()
  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# set correct libraries for OS
if(WIN32)
    set(GLFW_LIB "${PROJECT_SOURCE_DIR}/libs/glfw3/windows_x64/glfw3.lib")
elseif(APPLE)
    include(CMakeDetermineSystem)
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(GLFW_LIB "${ENGINE_DIR}/libs/glfw3/macos_arm64/libglfw3.a")
    else()
        set(GLFW_LIB "${ENGINE_DIR}/libs/gflw3/macos_x64/libglfw3.a")
    endif()
else()
    endif()



project(app)

add_executable(app src/application.cpp)

find_library(ENGINE_LIB BabyVulkanEngine)
target_link_libraries(app ${ENGINE_LIB})
target_include_directories(
	app PRIVATE 
	${ENGINE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    "${ENGINE_DIR}/libs/glfw3"
    "${ENGINE_DIR}/libs/glm"
    "${ENGINE_DIR}/vendor/imgui"
    "${ENGINE_DIR}/libs/tinyobjloader"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

