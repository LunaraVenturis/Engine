cmake_minimum_required(VERSION 3.20)

project(Engine C CXX)

option(ENABLE_VERBOSE_LOG "Enable verbose logging" ON)
option(ENABLE_DEBUG_LOG "Enable debug log" ON)
option(ENABLE_MEMORY_DEBUG_LOG "Enable memory debug log" ON)
option(ENG_VENDORED "Use vendored libraries" ON)
option(SDLTTF_VENDORED "Use vendored SDL_ttf" ${ENG_VENDORED})
set(MAKE_EXPORT_COMPILE_COMMANDS "Enable export compile commands" CACHE BOOL ON FORCE)

set(CMAKE_CXX_STANDARD 23)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

if(WIN32)
    message(STATUS "${PROJECT_NAME}: Windows platform detected")
    set(LUNARA_ENGINE_WIN32 ON CACHE BOOL "Build Windows backend" FORCE)
    set(LUNARA_ENGINE_X11 OFF CACHE BOOL "Build X11 backend" FORCE)
    set(LUNARA_ENGINE_WAYLAND OFF CACHE BOOL "Build Wayland backend" FORCE)
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    execute_process(
        COMMAND bash -c "echo $XDG_SESSION_TYPE"
        OUTPUT_VARIABLE SESSION_TYPE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "${PROJECT_NAME}: Session type: ${SESSION_TYPE}")
    if(SESSION_TYPE STREQUAL "x11")
        message(STATUS "${PROJECT_NAME}: X11 detected")
        set(LUNARA_ENGINE_X11 ON CACHE BOOL "Build X11 backend" FORCE)
        set(LUNARA_ENGINE_WAYLAND OFF CACHE BOOL "Build X11 backend" FORCE)
        set(LUNARA_ENGINE_WIN32 OFF CACHE BOOL "Build Windows backend" FORCE)
    elseif(SESSION_TYPE STREQUAL "wayland")
        message(STATUS "${PROJECT_NAME}: Wayland detected")
        set(LUNARA_ENGINE_X11 OFF CACHE BOOL "Build X11 backend" FORCE)
        set(LUNARA_ENGINE_WAYLAND ON CACHE BOOL "Build Wayland backend" FORCE)
        set(LUNARA_ENGINE_WIN32 OFF CACHE BOOL "Build Windows backend" FORCE)
        set(SDL_WAYLAND ON CACHE BOOL "" FORCE)
        set(SDL_X11 OFF CACHE BOOL "" FORCE)
        set(SDL_VULKAN ON CACHE BOOL "" FORCE)
    else()
        message(STATUS "${PROJECT_NAME}: Unknown session type: ${SESSION_TYPE}")
    endif()
endif()

if(ENG_VENDORED)
    add_subdirectory(Vendor/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(Vendor/SDL_ttf EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3main)
endif()

message("${PROJECT_NAME}: Adding source files")
file(GLOB_RECURSE SANDBOX_SOURCE_FILES ${CMAKE_SOURCE_DIR}/Sandbox/*.cpp ${CMAKE_SOURCE_DIR}/Sandbox/*.c)
file(GLOB_RECURSE SANDBOX_HEADER_FILES ${CMAKE_SOURCE_DIR}/Sandbox/*.hpp ${CMAKE_SOURCE_DIR}/Sandbox/*.h)

message("${CMAKE_SOURCE_DIR}")
message("${SANDBOX_SOURCE_FILES}")

file(GLOB_RECURSE ENGINE_SOURCE_FILES ./EngineLib/*.cpp ./EngineLib/*.c)
file(GLOB_RECURSE ENGINE_HEADER_FILES ./EngineLib/*.hpp ./EngineLib/*.h)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

set(GLM_INCLDUE_DIR "./Vendor/glm")

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

add_compile_definitions(EngineLib PRIVATE $<IF:$<CONFIG:Debug>,_DEBUG,_RELEASE>)


target_compile_definitions(EngineLib PRIVATE
    $<$<BOOL:${LUNARA_ENGINE_WIN32}>:_LUNARA_ENGINE_WIN32>
    $<$<BOOL:${LUNARA_ENGINE_X11}>:_LUNARA_ENGINE_X11>
    $<$<BOOL:${LUNARA_ENGINE_WAYLAND}>:_LUNARA_ENGINE_WAYLAND>
)
if(WIN32)
    ## assume user will install this in C:/VulkanSDK/ 
    #List All directories in the directory
    file(GLOB DIRECTORIES "C:/VulkanSDK/*")
    # Filter out only directories
    set(VERSION_NAME "")
    foreach(dir IN LISTS DIRECTORIES)
        if(IS_DIRECTORY ${dir})
            # Extract the folder name
            get_filename_component(VERSION_NAME ${dir} NAME)
            break() # Exit the loop after finding the first directory
        endif()
    endforeach()
    # Check if a folder was found
    if(VERSION_NAME STREQUAL "")
        message(FATAL_ERROR "No folder found in C:/VulkanSDK")
    else()
        message(STATUS "The folder name is: ${VERSION_NAME}")
    endif()
    set(VULKAN_INCLUDE_DIR "C:/VulkanSDK/${VERSION_NAME}/Include")
    ## __LibVulkan  DO NOT REMOVE THIS LINE IT IS VERY IMPORTANT FOR VULKAN LIBRARY MANAGEMENT
    set(VULKAN_LIBRARY "C:/VulkanSDK/${VERSION_NAME}/Lib/vulkan-1.lib")

else()
    find_package(Vulkan REQUIRED)
    set(VULKAN_INCLUDE_DIR ${Vulkan_INCLUDE_DIRS})
    set(VULKAN_LIBRARY ${Vulkan_LIBRARIES})

    if (LUNARA_ENGINE_X11)
        find_package(X11 REQUIRED)
        link_libraries(${X11_LIBRARIES})
    elif (LUNARA_ENGINE_WAYLAND)
        find_package(Wayland REQUIRED)
        link_libraries(${Wayland_LIBRARIES})
    endif()
    target_compile_definitions(EngineLib PRIVATE _LINUX)
endif()

target_include_directories(EngineLib PRIVATE ${CMAKE_SOURCE_DIR}/EngineLib)
target_include_directories(EngineLib PRIVATE ${CMAKE_SOURCE_DIR}/Vendor/include)
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
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/Vendor/glm")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/Vendor/include")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/Sandbox")
target_link_libraries(Sandbox PRIVATE EngineInterfaceLibrary EngineLib)
if(WIN32)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Sandbox> $<TARGET_FILE_DIR:Sandbox>
        COMMAND_EXPAND_LISTS
    )
endif()
function(target_add_flags target)
    if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic -Wno-nested-anon-types)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic)
    endif()
endfunction(target_add_flags)

if(WIN32)
    set(SHELL_INTERPRETER "powershell")
    set(SHELL_EXTENSION ".bat")
else()
    set(SHELL_INTERPRETER "/bin/bash")
    set(SHELL_EXTENSION ".sh")
endif()

add_custom_target(copy_assets COMMAND
    python3 ${CMAKE_SOURCE_DIR}/scripts/BuildShaders.py ${CMAKE_SOURCE_DIR}
    COMMENT "Copying and Building Assets"
)

add_dependencies(EngineLib copy_assets)
add_dependencies(Sandbox copy_assets)

target_add_flags(EngineLib)
target_add_flags(Sandbox)
