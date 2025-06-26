function(target_add_pedantic_flags target)
    if(CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic)

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target} PRIVATE -Wall -Wconversion -Werror -Wextra -Wpedantic)
    endif()
endfunction(target_add_pedantic_flags)
