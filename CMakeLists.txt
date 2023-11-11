cmake_minimum_required (VERSION 3.8)

project ("BabyVulkanEngine")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(Vulkan REQUIRED)

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
    "src/render_system.h" "src/render_system.cpp"
    "src/camera.h" "src/camera.cpp"
    "vendor/imgui/imgui.cpp" "vendor/imgui/imgui_demo.cpp" 
    "vendor/imgui/imgui_draw.cpp" "vendor/imgui/imgui_tables.cpp" 
    "vendor/imgui/imgui.h" "vendor/imgui/imgui_internal.h"
    "vendor/imgui/imstb_rectpack.h" "vendor/imgui/imstb_textedit.h"
    "vendor/imgui/imgui_widgets.cpp" "vendor/imgui/imstb_truetype.h"
    "vendor/imgui/imgui_impl_glfw.cpp" "vendor/imgui/imgui_impl_glfw.h" 
    "vendor/imgui/imgui_impl_vulkan.cpp" "vendor/imgui/imgui_impl_vulkan.h"
    "src/bve_imgui.h" "src/bve_imgui.cpp" 
    "src/movement_system.h" "src/movement_system.cpp"
    "src/camera_system.h" "src/camera_system.cpp"
    "src/input_controller.cpp"  "src/input_controller.h"
    "src/bve_utils.h" "src/vulkan_buffer.cpp"
    "src/vulkan_buffer.h" "src/frame_info.h" "src/vulkan_descriptors.h" "src/vulkan_descriptors.cpp")

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

# compile shaders
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.frag"
    "shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} Shaders)

# custom commands - copy resources to target
add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"

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

install( 
    TARGETS ${TARGET} 
    RUNTIME DESTINATION
)