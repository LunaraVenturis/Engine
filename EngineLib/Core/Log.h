#ifndef LOG_HEADER
#define LOG_HEADER
/**
 * @file
 * @author Krasto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2024 Krasto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * CLog Header
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#ifndef NO_STD_LOG
#include <stdio.h>
#endif
/***********************************************************************************************************************
Macro Definitions
***********************************************************************************************************************/

// clang-format off
#ifndef NO_STD_LOG
    #define LOG_WARNING( ... ) printf( __VA_ARGS__ )
    #define LOG( ... ) printf( __VA_ARGS__ );fflush(stdout)
    #define LOG_ERROR( ... )                                                                                               \
        LOG( "Error: " );                                                                                                  \
        printf( __VA_ARGS__ )
    
    #ifdef CUTILS_VERBOSE
        #define LOG_INFO( ... )                                                                                                \
            LOG( "Info: " );                                                                                                   \
            printf( __VA_ARGS__ )
    #else   
        #define LOG_INFO( ... )
    #endif
    #ifdef NDEBUG
        #define LOG_DEBUG(...)
    #else
        #define LOG_DEBUG(...)\
            LOG("Debug: ");\
            printf(__VA_ARGS__)
    #endif

#else
    #define LOG( ... )
    #define LOG_INFO( ... )
    #define LOG_DEBUG(...)

#endif
// clang-format on
/***********************************************************************************************************************
Static functions implementation
***********************************************************************************************************************/

#endif// CLOG_HEADER