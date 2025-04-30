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
 * Events implementation
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

#include "Events.h"
#include <SDL3/SDL.h>

/***********************************************************************************************************************
Type Declarations
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/

int PollEvents(Event* event)
{
    SDL_Event ev;
    int result = SDL_PollEvent(&ev);

    if (result != 0)
    {
        switch (ev.type)
        {
            case SDL_EVENT_QUIT:
                event->type = EVENT_QUIT;
                break;

            case SDL_EVENT_KEY_DOWN:
                event->type = EVENT_KEYBOARD;
                event->keyEvent.type = KEY_PRESSED;
                event->keyEvent.key = ev.key.key;
                break;

            case SDL_EVENT_KEY_UP:
                event->type = EVENT_KEYBOARD;
                event->keyEvent.type = KEY_RELEASED;
                event->keyEvent.key = ev.key.key;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event->type = EVENT_MOUSE_BUTTON;
                event->mouseButtonEvent.type = MOUSE_BUTTON_PRESSED;

                if (ev.button.button == SDL_BUTTON_LEFT) event->mouseButtonEvent.button = MOUSE_BUTTON_LEFT;
                else if (ev.button.button == SDL_BUTTON_MIDDLE)
                    event->mouseButtonEvent.button = MOUSE_BUTTON_MIDDLE;
                else if (ev.button.button == SDL_BUTTON_RIGHT)
                    event->mouseButtonEvent.button = MOUSE_BUTTON_RIGHT;
                event->mouseButtonEvent.x = ev.button.x;
                event->mouseButtonEvent.y = ev.button.y;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                event->type = EVENT_MOUSE_BUTTON;
                event->mouseButtonEvent.type = MOUSE_BUTTON_RELEASED;

                if (ev.button.button == SDL_BUTTON_LEFT) event->mouseButtonEvent.button = MOUSE_BUTTON_LEFT;
                else if (ev.button.button == SDL_BUTTON_MIDDLE)
                    event->mouseButtonEvent.button = MOUSE_BUTTON_MIDDLE;
                else if (ev.button.button == SDL_BUTTON_RIGHT)
                    event->mouseButtonEvent.button = MOUSE_BUTTON_RIGHT;
                event->mouseButtonEvent.x = ev.button.x;
                event->mouseButtonEvent.y = ev.button.y;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                event->type = EVENT_MOUSE_MOTION;
                event->mouseMotionEvent.x = ev.motion.x;
                event->mouseMotionEvent.y = ev.motion.y;
                event->mouseMotionEvent.xrel = ev.motion.xrel;
                event->mouseMotionEvent.yrel = ev.motion.yrel;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                event->type = EVENT_RESIZE_WINDOW;
                event->resizeWindowEvent.width = (uint32_t) ev.window.data1;
                event->resizeWindowEvent.height = (uint32_t) ev.window.data2;
                break;
        }
    }

    return result;
}
