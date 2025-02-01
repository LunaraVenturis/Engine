cmake_minimum_required(VERSION 3.20)

project(Engine C CXX)

option(ENABLE_VERBOSE_LOG "Enable verbose logging" ON)
option(ENABLE_DEBUG_LOG "Enable debug log" ON)
option(ENABLE_MEMORY_DEBUG_LOG "Enable memory debug log" ON)
option(ENG_VENDORED "Use vendored libraries" ON)
option(SDLTTF_VENDORED "Use vendored SDL_ttf" ${ENG_VENDORED})

set(CMAKE_CXX_STANDARD 23)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

if(ENG_VENDORED)
    add_subdirectory(Vendor/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(Vendor/SDL_ttf EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3main)
endif()

message("${PROJECT_NAME}: Adding source files")
file(GLOB_RECURSE SANDBOX_SOURCE_FILES ./Sandbox/*.cpp ./Sandbox/*.c)
file(GLOB_RECURSE SANDBOX_HEADER_FILES ./Sandbox/*.hpp ./Sandbox/*.h)

file(GLOB_RECURSE ENGINE_SOURCE_FILES ./EngineLib/*.cpp ./EngineLib/*.c)
file(GLOB_RECURSE ENGINE_HEADER_FILES ./EngineLib/*.hpp ./EngineLib/*.h)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

set(GLM_INCLDUE_DIR            "./Vendor/glm")

add_library(EngineLib STATIC ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES})
set_target_properties(EngineLib PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
    ## __VulkanInclude  DO NOT REMOVE THIS LINE IT IS VERY IMPORTANT FOR VULKAN LIBRARY MANAGEMENT
    set(VULKAN_INCLUDE_DIR    "C:/VulkanSDK/1.3.261.1/Include")
    ## __LibVulkan  DO NOT REMOVE THIS LINE IT IS VERY IMPORTANT FOR VULKAN LIBRARY MANAGEMENT
    set(VULKAN_LIBRARY "C:/VulkanSDK/1.3.261.1/Lib/vulkan-1.lib")

else ()
    find_package(Vulkan REQUIRED)
    set(VULKAN_INCLUDE_DIR  ${Vulkan_INCLUDE_DIRS})
    set(VULKAN_LIBRARY ${Vulkan_LIBRARIES})

    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
    target_compile_definitions(EngineLib PRIVATE _LINUX)
endif ()

target_include_directories(EngineLib PRIVATE "./EngineLib")
target_include_directories(EngineLib PUBLIC ${GLM_INCLDUE_DIR})
target_include_directories(EngineLib PUBLIC ${VULKAN_INCLUDE_DIR})

add_library(EngineInterfaceLibrary INTERFACE)

target_link_libraries(EngineInterfaceLibrary INTERFACE
        ${VULKAN_LIBRARY}
        )
target_link_libraries(EngineLib PRIVATE EngineInterfaceLibrary)

if(TARGET SDL3::SDL3main)
    target_link_libraries(EngineLib PUBLIC SDL3::SDL3main)
endif()
target_link_libraries(EngineLib PUBLIC SDL3::SDL3 SDL3_ttf::SDL3_ttf)

add_executable(Sandbox ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/EngineLib")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/Sandbox")
target_link_libraries(Sandbox PRIVATE EngineInterfaceLibrary EngineLib)

add_custom_command(TARGET Sandbox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Sandbox> $<TARGET_FILE_DIR:Sandbox>
  COMMAND_EXPAND_LISTS
  )

function(target_add_flags target)
if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic)
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic)
endif()
endfunction(target_add_flags)

target_add_flags(EngineLib)
target_add_flags(Sandbox)