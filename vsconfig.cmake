set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_options(
    $<$<CONFIG:>:/MT>
    $<$<CONFIG:Debug>:/MTd> 
    $<$<CONFIG:Release>:/MT>
)

set(target_list "" CACHE INTERNAL "target_list")
list(APPEND target_list_kind_to_skip "INTERFACE" "ALIAS")
list(APPEND target_kinds "INTERFACE" "ALIAS" "OBJECT" "STATIC" "SHARED" "MODULE")
list(APPEND our_targets_to_skip "EngineLib" "Sandbox")
function(add_library name kind)
    message("Adding library ${name}")
    if("${name}" IN_LIST our_targets_to_skip)
    elseif("${kind}" IN_LIST target_list_kind_to_skip)
    else()  
        set(target_list ${target_list} ${name} CACHE INTERNAL "target_list")
    endif()
    _add_library(${name} ${kind} ${ARGN})

endfunction()

function(add_custom_target name)
    message("Adding custom target ${name}")
    set(target_list ${target_list} ${name} CACHE INTERNAL "target_list")
    _add_custom_target(${name} ${ARGN})
endfunction()

function(add_executable name)
    message("Adding executable ${name}")
    if("${name}" IN_LIST our_targets_to_skip)
    elseif("${name}" MATCHES "Python")
    else()
        set(target_list ${target_list} ${name} CACHE INTERNAL "target_list")
    endif()
    _add_executable(${name} ${ARGN})
endfunction()

function(filter_targets)
    message("Adding all libraries to Vendor filter")
    set_target_properties(${target_list} PROPERTIES FOLDER "Vendor")
endfunction()