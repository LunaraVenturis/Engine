#ifndef TIMESTEP_HEADER
#define TIMESTEP_HEADER

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto
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
 * Timestep
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "STDTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************************************************************
Type Declarations
***********************************************************************************************************************/
typedef enum
{
    TIME_MS = 0,
    TIME_S
} TimeUnit;

typedef struct {
    double value;
    TimeUnit unit;
} Time;

typedef struct {
    long start;
    long end;
    double t;
    double t_ms;
} Timer;

/***********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/
extern void Timer_Start(Timer* timer);
extern void Timer_End(Timer* timer);
extern void Timer_Wait(Timer* timer, long milliseconds);

#ifdef __cplusplus
}
#endif
#endif
// Timestep_HEADER
