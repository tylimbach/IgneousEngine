include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required (VERSION 3.11)

project ("BabyVulkanEngine")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

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
    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/libs/glfw3/windows_x64/glfw3.lib")
elseif(APPLE)
    include(CMakeDetermineSystem)
    if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(GLFW_LIB "${CMAKE_SOURCE_DIR}/libs/glfw3/macos_arm64/libglfw3.a")
    else()
        set(GLFW_LIB "${CMAKE_SOURCE_DIR}/libs/gflw3/macos_x64/libglfw3.a")
    endif()
else()
    endif()

# create exec
add_executable(
    ${PROJECT_NAME} 
    "src/bve_window.cpp" "src/basic_app.h"
    "src/basic_app.cpp" "src/main.cpp"
    "src/bve_pipeline.h" "src/bve_pipeline.cpp"
    "src/bve_device.cpp" "src/bve_device.h"
    "src/bve_swap_chain.cpp" "src/bve_swap_chain.h" 
    "src/bve_model.h" "src/bve_model.cpp" 
    "src/entity_manager.h" 
    "src/entity_component_registry.h"
    "src/components/components.h"
    "src/entity_component_view.h" 
    "src/renderer.h" "src/renderer.cpp"
    "src/systems/render_system.h" "src/systems/render_system.cpp"
    "src/camera.h" "src/camera.cpp"
    "vendor/imgui/imgui.cpp" "vendor/imgui/imgui_demo.cpp" 
    "vendor/imgui/imgui_draw.cpp" "vendor/imgui/imgui_tables.cpp" 
    "vendor/imgui/imgui.h" "vendor/imgui/imgui_internal.h"
    "vendor/imgui/imstb_rectpack.h" "vendor/imgui/imstb_textedit.h"
    "vendor/imgui/imgui_widgets.cpp" "vendor/imgui/imstb_truetype.h"
    "vendor/imgui/imgui_impl_glfw.cpp" "vendor/imgui/imgui_impl_glfw.h" 
    "vendor/imgui/imgui_impl_vulkan.cpp" "vendor/imgui/imgui_impl_vulkan.h"
    "src/bve_imgui.h" "src/bve_imgui.cpp" 
    "src/systems/movement_system.h" "src/systems/movement_system.cpp"
    "src/systems/camera_system.h" "src/systems/camera_system.cpp"
    "src/input_controller.cpp"  "src/input_controller.h"
    "src/bve_utils.h" "src/vulkan_buffer.cpp"
    "src/vulkan_buffer.h" "src/frame_info.h" "src/vulkan_descriptors.h" "src/vulkan_descriptors.cpp" "src/systems/point_light_render_system.cpp" "src/systems/point_light_render_system.h")

# includes
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/libs/glfw3"
    "${PROJECT_SOURCE_DIR}/libs/glm"
    "${PROJECT_SOURCE_DIR}/vendor/imgui"
    "${PROJECT_SOURCE_DIR}/libs/tinyobjloader"
)

# link libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
)

############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)
 
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)
 
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
    message(STATUS "SPIR-V file will be generated: ${SPIRV}")
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

# custom commands - copy resources to target
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy
        ${SPIRV_BINARY_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/models"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATH})
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE )
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

add_dependencies(${PROJECT_NAME} Shaders)
