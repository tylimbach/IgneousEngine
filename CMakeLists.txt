cmake_minimum_required (VERSION 3.8)

project ("BabyVulkanEngine")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Find the packages
# set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.261.1")
find_package(Vulkan REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glm REQUIRED)

# Create an executable
add_executable(
    ${PROJECT_NAME} 
    "src/bve_window.cpp" "src/basic_app.h"
    "src/basic_app.cpp" "src/main.cpp"
    "src/bve_pipeline.h" "src/bve_pipeline.cpp"
    "src/bve_device.cpp" "src/bve_device.h"
    "src/bve_swap_chain.cpp" "src/bve_swap_chain.h" 
    "src/bve_model.h" "src/bve_model.cpp")

# Include the directories
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
)

# Link the libraries to your project
target_link_libraries(
    ${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    ${GLFW3_LIBRARY}
    ${GLM_LIBRARIES}
)

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

add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
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