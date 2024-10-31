cmake_minimum_required(VERSION 3.20)

project(Engine CXX)

option(ENABLE_VERBOSE_LOG "Enable verbose logging" ON)
option(ENABLE_DEBUG_LOG "Enable debug log" ON)
option(ENABLE_MEMORY_DEBUG_LOG "Enable memory debug log" ON)

set(CMAKE_CXX_STANDARD 20)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

message("${PROJECT_NAME}: Adding glfw...")
add_subdirectory("${CMAKE_SOURCE_DIR}/Vendor/glfw-3.4")

message("${PROJECT_NAME}: Adding source files")
file(GLOB_RECURSE SANDBOX_SOURCE_FILES ./Sandbox/src/*.cpp)
file(GLOB_RECURSE SANDBOX_HEADER_FILES ./Sandbox/src/*.hpp)

file(GLOB_RECURSE ENGINE_SOURCE_FILES ./EngineLib/src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADER_FILES ./EngineLib/src/*.hpp)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

set(GLM_INCLDUE_DIR            "./Vendor/glm")
set(GLFW_INCLDUE_DIR           "./Vendor/glfw-3.4/include")

## __Vulkan  DO NOT REMOVE THIS LINE IT IS VERY IMPORTANT FOR VULKAN LIBRARY MANAGEMENT
set(GLFW_VULKAN_INCLUDE_DIR    "C:/VulkanSDK/1.3.296.0")

add_library(EngineLib STATIC ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES})
set_target_properties(EngineLib PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(EngineLib PRIVATE -static-libstdc++)
    target_compile_options(EngineLib PRIVATE -Wall)
    target_compile_options(EngineLib PRIVATE -pedantic)
endif()

if(ENABLE_VERBOSE_LOG)
    add_compile_definitions(EngineLib PRIVATE ENGINE_ENABLE_VERBOSE_LOG)
endif()

if(ENABLE_DEBUG_LOG)
    add_compile_definitions(EngineLib PRIVATE ENGINE_ENABLE_DEBUG_LOG)
endif()

if(ENABLE_MEMORY_DEBUG_LOG)
    add_compile_definitions(EngineLib PRIVATE ENGINE_ENABLE_MEMORY_DEBUG_LOG)
endif()

if (WIN32)
    target_compile_definitions(EngineLib PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
else ()
    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
    target_compile_definitions(EngineLib PRIVATE _LINUX)
    target_compile_definitions(EngineLib PRIVATE GLFW_EXPOSE_NATIVE_X11)
endif ()

target_include_directories(EngineLib PRIVATE "./EngineLib/src")
target_include_directories(EngineLib PUBLIC ${GLFW_INCLDUE_DIR})
target_include_directories(EngineLib PUBLIC ${GLM_INCLDUE_DIR})

add_library(EngineInterfaceLibrary INTERFACE)

target_link_libraries(EngineInterfaceLibrary INTERFACE
        glfw
        )
target_link_libraries(EngineLib PUBLIC EngineInterfaceLibrary)

add_executable(Sandbox ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/EngineLib/src")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/Sandbox/src")
target_link_libraries(Sandbox EngineLib)
