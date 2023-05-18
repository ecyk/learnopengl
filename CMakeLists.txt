cmake_minimum_required(VERSION 3.11)

project(learnopengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB SOURCES common/*.cpp)
add_library(common STATIC ${SOURCES})
target_include_directories(common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/external/include
)
target_link_libraries(common glm::glm assimp::assimp)

function(make_executable TARGET)
    file(GLOB SOURCES ${TARGET}/*.cpp)
    add_executable(${TARGET} ${SOURCES})

    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${TARGET})
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/${TARGET})
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/${TARGET})
    set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${TARGET})

    target_link_libraries(${TARGET} common glfw)

    file(GLOB SHADERS ${TARGET}/*.vert ${TARGET}/*.frag)
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${SHADER} ${CMAKE_SOURCE_DIR}/bin/${TARGET}/${SHADER_NAME}
        )
    endforeach()

    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR}/bin/${TARGET}/resources
    )
endfunction()

make_executable(1.0.hello_window)
make_executable(1.1.hello_triangle)
make_executable(1.2.shaders)
make_executable(1.3.textures)
make_executable(1.4.transformations)
make_executable(1.5.coordinate_systems)
make_executable(1.6.camera)

make_executable(2.0.colors)
make_executable(2.1.basic_lighting)
make_executable(2.2.materials)
make_executable(2.3.lighting_maps)
make_executable(2.4.light_casters)
make_executable(2.5.multiple_lights)
make_executable(3.0.model_loading)
