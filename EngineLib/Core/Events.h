#ifndef EVENTS_HEADER
#define EVENTS_HEADER

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
 * Events management
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#pragma once
#include "STDTypes.h"
#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************************************************************
Macro Definitions
***********************************************************************************************************************/
#define KEY_UNKNOWN 0x00000000u              /**< 0 */
#define KEY_RETURN 0x0000000du               /**< '\r' */
#define KEY_ESCAPE 0x0000001bu               /**< '\x1B' */
#define KEY_BACKSPACE 0x00000008u            /**< '\b' */
#define KEY_TAB 0x00000009u                  /**< '\t' */
#define KEY_SPACE 0x00000020u                /**< ' ' */
#define KEY_EXCLAIM 0x00000021u              /**< '!' */
#define KEY_DBLAPOSTROPHE 0x00000022u        /**< '"' */
#define KEY_HASH 0x00000023u                 /**< '#' */
#define KEY_DOLLAR 0x00000024u               /**< '$' */
#define KEY_PERCENT 0x00000025u              /**< '%' */
#define KEY_AMPERSAND 0x00000026u            /**< '&' */
#define KEY_APOSTROPHE 0x00000027u           /**< '\'' */
#define KEY_LEFTPAREN 0x00000028u            /**< '(' */
#define KEY_RIGHTPAREN 0x00000029u           /**< ')' */
#define KEY_ASTERISK 0x0000002au             /**< '*' */
#define KEY_PLUS 0x0000002bu                 /**< '+' */
#define KEY_COMMA 0x0000002cu                /**< ',' */
#define KEY_MINUS 0x0000002du                /**< '-' */
#define KEY_PERIOD 0x0000002eu               /**< '.' */
#define KEY_SLASH 0x0000002fu                /**< '/' */
#define KEY_0 0x00000030u                    /**< '0' */
#define KEY_1 0x00000031u                    /**< '1' */
#define KEY_2 0x00000032u                    /**< '2' */
#define KEY_3 0x00000033u                    /**< '3' */
#define KEY_4 0x00000034u                    /**< '4' */
#define KEY_5 0x00000035u                    /**< '5' */
#define KEY_6 0x00000036u                    /**< '6' */
#define KEY_7 0x00000037u                    /**< '7' */
#define KEY_8 0x00000038u                    /**< '8' */
#define KEY_9 0x00000039u                    /**< '9' */
#define KEY_COLON 0x0000003au                /**< ':' */
#define KEY_SEMICOLON 0x0000003bu            /**< ';' */
#define KEY_LESS 0x0000003cu                 /**< '<' */
#define KEY_EQUALS 0x0000003du               /**< '=' */
#define KEY_GREATER 0x0000003eu              /**< '>' */
#define KEY_QUESTION 0x0000003fu             /**< '?' */
#define KEY_AT 0x00000040u                   /**< '@' */
#define KEY_LEFTBRACKET 0x0000005bu          /**< '[' */
#define KEY_BACKSLASH 0x0000005cu            /**< '\\' */
#define KEY_RIGHTBRACKET 0x0000005du         /**< ']' */
#define KEY_CARET 0x0000005eu                /**< '^' */
#define KEY_UNDERSCORE 0x0000005fu           /**< '_' */
#define KEY_GRAVE 0x00000060u                /**< '`' */
#define KEY_A 0x00000061u                    /**< 'a' */
#define KEY_B 0x00000062u                    /**< 'b' */
#define KEY_C 0x00000063u                    /**< 'c' */
#define KEY_D 0x00000064u                    /**< 'd' */
#define KEY_E 0x00000065u                    /**< 'e' */
#define KEY_F 0x00000066u                    /**< 'f' */
#define KEY_G 0x00000067u                    /**< 'g' */
#define KEY_H 0x00000068u                    /**< 'h' */
#define KEY_I 0x00000069u                    /**< 'i' */
#define KEY_J 0x0000006au                    /**< 'j' */
#define KEY_K 0x0000006bu                    /**< 'k' */
#define KEY_L 0x0000006cu                    /**< 'l' */
#define KEY_M 0x0000006du                    /**< 'm' */
#define KEY_N 0x0000006eu                    /**< 'n' */
#define KEY_O 0x0000006fu                    /**< 'o' */
#define KEY_P 0x00000070u                    /**< 'p' */
#define KEY_Q 0x00000071u                    /**< 'q' */
#define KEY_R 0x00000072u                    /**< 'r' */
#define KEY_S 0x00000073u                    /**< 's' */
#define KEY_T 0x00000074u                    /**< 't' */
#define KEY_U 0x00000075u                    /**< 'u' */
#define KEY_V 0x00000076u                    /**< 'v' */
#define KEY_W 0x00000077u                    /**< 'w' */
#define KEY_X 0x00000078u                    /**< 'x' */
#define KEY_Y 0x00000079u                    /**< 'y' */
#define KEY_Z 0x0000007au                    /**< 'z' */
#define KEY_LEFTBRACE 0x0000007bu            /**< '{' */
#define KEY_PIPE 0x0000007cu                 /**< '|' */
#define KEY_RIGHTBRACE 0x0000007du           /**< '}' */
#define KEY_TILDE 0x0000007eu                /**< '~' */
#define KEY_DELETE 0x0000007fu               /**< '\x7F' */
#define KEY_PLUSMINUS 0x000000b1u            /**< '\xB1' */
#define KEY_CAPSLOCK 0x40000039u             /**< */
#define KEY_F1 0x4000003au                   /**< */
#define KEY_F2 0x4000003bu                   /**< */
#define KEY_F3 0x4000003cu                   /**< */
#define KEY_F4 0x4000003du                   /**< */
#define KEY_F5 0x4000003eu                   /**< */
#define KEY_F6 0x4000003fu                   /**< */
#define KEY_F7 0x40000040u                   /**< */
#define KEY_F8 0x40000041u                   /**< */
#define KEY_F9 0x40000042u                   /**< */
#define KEY_F10 0x40000043u                  /**< */
#define KEY_F11 0x40000044u                  /**< */
#define KEY_F12 0x40000045u                  /**< */
#define KEY_PRINTSCREEN 0x40000046u          /**< */
#define KEY_SCROLLLOCK 0x40000047u           /**< */
#define KEY_PAUSE 0x40000048u                /**< */
#define KEY_INSERT 0x40000049u               /**< */
#define KEY_HOME 0x4000004au                 /**< */
#define KEY_PAGEUP 0x4000004bu               /**< */
#define KEY_END 0x4000004du                  /**< */
#define KEY_PAGEDOWN 0x4000004eu             /**< */
#define KEY_RIGHT 0x4000004fu                /**< */
#define KEY_LEFT 0x40000050u                 /**< */
#define KEY_DOWN 0x40000051u                 /**< */
#define KEY_UP 0x40000052u                   /**< */
#define KEY_NUMLOCKCLEAR 0x40000053u         /**< */
#define KEY_KP_DIVIDE 0x40000054u            /**< */
#define KEY_KP_MULTIPLY 0x40000055u          /**< */
#define KEY_KP_MINUS 0x40000056u             /**< */
#define KEY_KP_PLUS 0x40000057u              /**< */
#define KEY_KP_ENTER 0x40000058u             /**< */
#define KEY_KP_1 0x40000059u                 /**< */
#define KEY_KP_2 0x4000005au                 /**< */
#define KEY_KP_3 0x4000005bu                 /**< */
#define KEY_KP_4 0x4000005cu                 /**< */
#define KEY_KP_5 0x4000005du                 /**< */
#define KEY_KP_6 0x4000005eu                 /**< */
#define KEY_KP_7 0x4000005fu                 /**< */
#define KEY_KP_8 0x40000060u                 /**< */
#define KEY_KP_9 0x40000061u                 /**< */
#define KEY_KP_0 0x40000062u                 /**< */
#define KEY_KP_PERIOD 0x40000063u            /**< */
#define KEY_APPLICATION 0x40000065u          /**< */
#define KEY_POWER 0x40000066u                /**< */
#define KEY_KP_EQUALS 0x40000067u            /**< */
#define KEY_F13 0x40000068u                  /**< */
#define KEY_F14 0x40000069u                  /**< */
#define KEY_F15 0x4000006au                  /**< */
#define KEY_F16 0x4000006bu                  /**< */
#define KEY_F17 0x4000006cu                  /**< */
#define KEY_F18 0x4000006du                  /**< */
#define KEY_F19 0x4000006eu                  /**< */
#define KEY_F20 0x4000006fu                  /**< */
#define KEY_F21 0x40000070u                  /**< */
#define KEY_F22 0x40000071u                  /**< */
#define KEY_F23 0x40000072u                  /**< */
#define KEY_F24 0x40000073u                  /**< */
#define KEY_EXECUTE 0x40000074u              /**< */
#define KEY_HELP 0x40000075u                 /**< */
#define KEY_MENU 0x40000076u                 /**< */
#define KEY_SELECT 0x40000077u               /**< */
#define KEY_STOP 0x40000078u                 /**< */
#define KEY_AGAIN 0x40000079u                /**< */
#define KEY_UNDO 0x4000007au                 /**< */
#define KEY_CUT 0x4000007bu                  /**< */
#define KEY_COPY 0x4000007cu                 /**< */
#define KEY_PASTE 0x4000007du                /**< */
#define KEY_FIND 0x4000007eu                 /**< */
#define KEY_MUTE 0x4000007fu                 /**< */
#define KEY_VOLUMEUP 0x40000080u             /**< */
#define KEY_VOLUMEDOWN 0x40000081u           /**< */
#define KEY_KP_COMMA 0x40000085u             /**< */
#define KEY_KP_EQUALSAS400 0x40000086u       /**< */
#define KEY_ALTERASE 0x40000099u             /**< */
#define KEY_SYSREQ 0x4000009au               /**< */
#define KEY_CANCEL 0x4000009bu               /**< */
#define KEY_CLEAR 0x4000009cu                /**< */
#define KEY_PRIOR 0x4000009du                /**< */
#define KEY_RETURN2 0x4000009eu              /**< */
#define KEY_SEPARATOR 0x4000009fu            /**< */
#define KEY_OUT 0x400000a0u                  /**< */
#define KEY_OPER 0x400000a1u                 /**< */
#define KEY_CLEARAGAIN 0x400000a2u           /**< */
#define KEY_CRSEL 0x400000a3u                /**< */
#define KEY_EXSEL 0x400000a4u                /**< */
#define KEY_KP_00 0x400000b0u                /**< */
#define KEY_KP_000 0x400000b1u               /**< */
#define KEY_THOUSANDSSEPARATOR 0x400000b2u   /**< */
#define KEY_DECIMALSEPARATOR 0x400000b3u     /**< */
#define KEY_CURRENCYUNIT 0x400000b4u         /**< */
#define KEY_CURRENCYSUBUNIT 0x400000b5u      /**< */
#define KEY_KP_LEFTPAREN 0x400000b6u         /**< */
#define KEY_KP_RIGHTPAREN 0x400000b7u        /**< */
#define KEY_KP_LEFTBRACE 0x400000b8u         /**< */
#define KEY_KP_RIGHTBRACE 0x400000b9u        /**< */
#define KEY_KP_TAB 0x400000bau               /**< */
#define KEY_KP_BACKSPACE 0x400000bbu         /**< */
#define KEY_KP_A 0x400000bcu                 /**< */
#define KEY_KP_B 0x400000bdu                 /**< */
#define KEY_KP_C 0x400000beu                 /**< */
#define KEY_KP_D 0x400000bfu                 /**< */
#define KEY_KP_E 0x400000c0u                 /**< */
#define KEY_KP_F 0x400000c1u                 /**< */
#define KEY_KP_XOR 0x400000c2u               /**< */
#define KEY_KP_POWER 0x400000c3u             /**< */
#define KEY_KP_PERCENT 0x400000c4u           /**< */
#define KEY_KP_LESS 0x400000c5u              /**< */
#define KEY_KP_GREATER 0x400000c6u           /**< */
#define KEY_KP_AMPERSAND 0x400000c7u         /**< */
#define KEY_KP_DBLAMPERSAND 0x400000c8u      /**< */
#define KEY_KP_VERTICALBAR 0x400000c9u       /**< */
#define KEY_KP_DBLVERTICALBAR 0x400000cau    /**< */
#define KEY_KP_COLON 0x400000cbu             /**< */
#define KEY_KP_HASH 0x400000ccu              /**< */
#define KEY_KP_SPACE 0x400000cdu             /**< */
#define KEY_KP_AT 0x400000ceu                /**< */
#define KEY_KP_EXCLAM 0x400000cfu            /**< */
#define KEY_KP_MEMSTORE 0x400000d0u          /**< */
#define KEY_KP_MEMRECALL 0x400000d1u         /**< */
#define KEY_KP_MEMCLEAR 0x400000d2u          /**< */
#define KEY_KP_MEMADD 0x400000d3u            /**< */
#define KEY_KP_MEMSUBTRACT 0x400000d4u       /**< */
#define KEY_KP_MEMMULTIPLY 0x400000d5u       /**< */
#define KEY_KP_MEMDIVIDE 0x400000d6u         /**< */
#define KEY_KP_PLUSMINUS 0x400000d7u         /**< */
#define KEY_KP_CLEAR 0x400000d8u             /**< */
#define KEY_KP_CLEARENTRY 0x400000d9u        /**< */
#define KEY_KP_BINARY 0x400000dau            /**< */
#define KEY_KP_OCTAL 0x400000dbu             /**< */
#define KEY_KP_DECIMAL 0x400000dcu           /**< */
#define KEY_KP_HEXADECIMAL 0x400000ddu       /**< */
#define KEY_LCTRL 0x400000e0u                /**< */
#define KEY_LSHIFT 0x400000e1u               /**< */
#define KEY_LALT 0x400000e2u                 /**< */
#define KEY_LGUI 0x400000e3u                 /**< */
#define KEY_RCTRL 0x400000e4u                /**< */
#define KEY_RSHIFT 0x400000e5u               /**< */
#define KEY_RALT 0x400000e6u                 /**< */
#define KEY_RGUI 0x400000e7u                 /**< */
#define KEY_MODE 0x40000101u                 /**< */
#define KEY_SLEEP 0x40000102u                /**< */
#define KEY_WAKE 0x40000103u                 /**< */
#define KEY_CHANNEL_INCREMENT 0x40000104u    /**< */
#define KEY_CHANNEL_DECREMENT 0x40000105u    /**< */
#define KEY_MEDIA_PLAY 0x40000106u           /**< */
#define KEY_MEDIA_PAUSE 0x40000107u          /**< */
#define KEY_MEDIA_RECORD 0x40000108u         /**< */
#define KEY_MEDIA_FAST_FORWARD 0x40000109u   /**< */
#define KEY_MEDIA_REWIND 0x4000010au         /**< */
#define KEY_MEDIA_NEXT_TRACK 0x4000010bu     /**< */
#define KEY_MEDIA_PREVIOUS_TRACK 0x4000010cu /**< */
#define KEY_MEDIA_STOP 0x4000010du           /**< */
#define KEY_MEDIA_EJECT 0x4000010eu          /**< */
#define KEY_MEDIA_PLAY_PAUSE 0x4000010fu     /**< */
#define KEY_MEDIA_SELECT 0x40000110u         /**< */
#define KEY_AC_NEW 0x40000111u               /**< */
#define KEY_AC_OPEN 0x40000112u              /**< */
#define KEY_AC_CLOSE 0x40000113u             /**< */
#define KEY_AC_EXIT 0x40000114u              /**< */
#define KEY_AC_SAVE 0x40000115u              /**< */
#define KEY_AC_PRINT 0x40000116u             /**< */
#define KEY_AC_PROPERTIES 0x40000117u        /**< */
#define KEY_AC_SEARCH 0x40000118u            /**< */
#define KEY_AC_HOME 0x40000119u              /**< */
#define KEY_AC_BACK 0x4000011au              /**< */
#define KEY_AC_FORWARD 0x4000011bu           /**< */
#define KEY_AC_STOP 0x4000011cu              /**< */
#define KEY_AC_REFRESH 0x4000011du           /**< */
#define KEY_AC_BOOKMARKS 0x4000011eu         /**< */
#define KEY_SOFTLEFT 0x4000011fu             /**< */
#define KEY_SOFTRIGHT 0x40000120u            /**< */
#define KEY_CALL 0x40000121u                 /**< */
#define KEY_ENDCALL 0x40000122u              /**< */
#define KEY_LEFT_TAB 0x20000001u             /**< Extended key Left Tab */
#define KEY_LEVEL5_SHIFT 0x20000002u         /**< Extended key Level 5 Shift */
#define KEY_MULTI_KEY_COMPOSE 0x20000003u    /**< Extended key Multi-key Compose */
#define KEY_LMETA 0x20000004u                /**< Extended key Left Meta */
#define KEY_RMETA 0x20000005u                /**< Extended key Right Meta */
#define KEY_LHYPER 0x20000006u               /**< Extended key Left Hyper */
#define KEY_RHYPER 0x20000007u               /**< Extended key Right Hyper */

/***********************************************************************************************************************
Type Declarations
***********************************************************************************************************************/
typedef enum
{
    EVENT_NONE = 0,
    EVENT_QUIT,
    EVENT_KEYBOARD,
    EVENT_MOUSE,
    EVENT_MOUSE_MOTION,
    EVENT_MOUSE_BUTTON,
    EVENT_RESIZE_WINDOW
} EventType;

typedef enum
{
    KEY_PRESSED = 0,
    KEY_RELEASED,
} KeyEventType;

typedef enum
{
    MOUSE_BUTTON_PRESSED = 0,
    MOUSE_BUTTON_RELEASED,
} MouseButtonEventType;

typedef enum
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
} MouseButton;

typedef struct {
    float x;
    float y;
    MouseButtonEventType type;
    MouseButton button;
} MouseButtonEvent;

typedef struct {
    float x;
    float y;
    float xrel;
    float yrel;
} MouseMotionEvent;

typedef struct {
    uint32_t key;
    KeyEventType type;
} KeyEvent;

typedef struct {
    uint32_t width;
    uint32_t height;
} ResizeWindowEvent;

typedef struct {
    EventType type;

    KeyEvent keyEvent;
    MouseButtonEvent mouseButtonEvent;
    MouseMotionEvent mouseMotionEvent;
    ResizeWindowEvent resizeWindowEvent;
} Event;

/***********************************************************************************************************************
Function Prototypes
***********************************************************************************************************************/

extern int PollEvents(Event* event);

#ifdef __cplusplus
}
#endif
#endif
// Events_HEADER
