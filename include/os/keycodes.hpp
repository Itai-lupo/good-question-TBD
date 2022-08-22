#pragma once
#include <cstdint>
#include <string>

enum class keycodes
{
    keySpace = 32,
    keyApostrophe = 39, /* ' */
    keyComma = 44,      /* , */
    keyMinus = 45,      /* - */
    keyPeriod = 46,     /* . */
    keySlash = 47,      /* / */
    key0 = 48,
    key1 = 49,
    key2 = 50,
    key3 = 51,
    key4 = 52,
    key5 = 53,
    key6 = 54,
    key7 = 55,
    key8 = 56,
    key9 = 57,
    keySemicolon = 59, /* ; */
    keyEqual = 61,     /* = */
    keyA = 65,
    keyB = 66,
    keyC = 67,
    keyD = 68,
    keyE = 69,
    keyF = 70,
    keyG = 71,
    keyH = 72,
    keyI = 73,
    keyJ = 74,
    keyK = 75,
    keyL = 76,
    keyM = 77,
    keyN = 78,
    keyO = 79,
    keyP = 80,
    keyQ = 81,
    keyR = 82,
    keyS = 83,
    keyT = 84,
    keyU = 85,
    keyV = 86,
    keyW = 87,
    keyX = 88,
    keyY = 89,
    keyZ = 90,
    keyLeftBracket = 91,  /* [ */
    keyBackslash = 92,     /* \ */
    keyRightBracket = 93, /* ] */
    keyGraveAccent = 96,  /* ` */
    keyWorld1 = 161,      /* non-US #1 */
    keyWorld2 = 162,      /* non-US #2 */

    /* Function keys */
    keyEscape = 256,
    keyEnter = 257,
    keyTab = 258,
    keyBackspace = 259,
    keyInsert = 260,
    keyDelete = 261,
    keyRight = 262,
    keyLeft = 263,
    keyDown = 264,
    keyUp = 265,
    keyPageUp = 266,
    keyPageDown = 267,
    keyHome = 268,
    keyEnd = 269,
    keyCapsLock = 280,
    keyScrollLock = 281,
    keyNumLock = 282,
    keyPrintScreen = 283,
    keyPause = 284,
    keyF1 = 290,
    keyF2 = 291,
    keyF3 = 292,
    keyF4 = 293,
    keyF5 = 294,
    keyF6 = 295,
    keyF7 = 296,
    keyF8 = 297,
    keyF9 = 298,
    keyF10 = 299,
    keyF11 = 300,
    keyF12 = 301,
    keyF13 = 302,
    keyF14 = 303,
    keyF15 = 304,
    keyF16 = 305,
    keyF17 = 306,
    keyF18 = 307,
    keyF19 = 308,
    keyF20 = 309,
    keyF21 = 310,
    keyF22 = 311,
    keyF23 = 312,
    keyF24 = 313,
    keyF25 = 314,
    keyKp0 = 320,
    keyKp1 = 321,
    keyKp2 = 322,
    keyKp3 = 323,
    keyKp4 = 324,
    keyKp5 = 325,
    keyKp6 = 326,
    keyKp7 = 327,
    keyKp8 = 328,
    keyKp9 = 329,
    keyKpDecimal = 330,
    keyKpDivide = 331,
    keyKpMultiply = 332,
    keyKpSubtract = 333,
    keyKpAdd = 334,
    keyKpEnter = 335,
    keyKpEqual = 336,
    keyLeftShift = 340,
    keyLeftControl = 341,
    keyLeftAlt = 342,
    keyLeftSuper = 343,
    keyRightShift = 344,
    keyRightControl = 345,
    keyRightAlt = 346,
    keyRightSuper = 347,
    keyMenu = 348,
    Last = keyMenu
};

static std::string keyCodeToString(keycodes value)
{
    switch (value)
    {
        case (keycodes::keySpace):
            return "Space";
            break;
        case (keycodes::keyApostrophe):
            return "Apostrophe";
            break;
        case (keycodes::keyComma):
            return "Comma";
            break;
        case (keycodes::keyMinus):
            return "Minus";
            break;
        case (keycodes::keyPeriod):
            return "Period";
            break;
        case (keycodes::keySlash):
            return "Slash";
            break;
        case (keycodes::key0):
            return "0";
            break;
        case (keycodes::key1):
            return "1";
            break;
        case (keycodes::key2):
            return "2";
            break;
        case (keycodes::key3):
            return "3";
            break;
        case (keycodes::key4):
            return "4";
            break;
        case (keycodes::key5):
            return "5";
            break;
        case (keycodes::key6):
            return "6";
            break;
        case (keycodes::key7):
            return "7";
            break;
        case (keycodes::key8):
            return "8";
            break;
        case (keycodes::key9):
            return "9";
            break;
        case (keycodes::keySemicolon):
            return "Semicolon";
            break;
        case (keycodes::keyEqual):
            return "Equal";
            break;
        case (keycodes::keyA):
            return "A";
            break;
        case (keycodes::keyB):
            return "B";
            break;
        case (keycodes::keyC):
            return "C";
            break;
        case (keycodes::keyD):
            return "D";
            break;
        case (keycodes::keyE):
            return "E";
            break;
        case (keycodes::keyF):
            return "F";
            break;
        case (keycodes::keyG):
            return "G";
            break;
        case (keycodes::keyH):
            return "H";
            break;
        case (keycodes::keyI):
            return "I";
            break;
        case (keycodes::keyJ):
            return "J";
            break;
        case (keycodes::keyK):
            return "K";
            break;
        case (keycodes::keyL):
            return "L";
            break;
        case (keycodes::keyM):
            return "M";
            break;
        case (keycodes::keyN):
            return "N";
            break;
        case (keycodes::keyO):
            return "O";
            break;
        case (keycodes::keyP):
            return "P";
            break;
        case (keycodes::keyQ):
            return "Q";
            break;
        case (keycodes::keyR):
            return "R";
            break;
        case (keycodes::keyS):
            return "S";
            break;
        case (keycodes::keyT):
            return "T";
            break;
        case (keycodes::keyU):
            return "U";
            break;
        case (keycodes::keyV):
            return "V";
            break;
        case (keycodes::keyW):
            return "W";
            break;
        case (keycodes::keyX):
            return "X";
            break;
        case (keycodes::keyY):
            return "Y";
            break;
        case (keycodes::keyZ):
            return "Z";
            break;
        case (keycodes::keyLeftBracket):
            return "LeftBracket";
            break;
        case (keycodes::keyBackslash):
            return "Backslash";
            break;
        case (keycodes::keyRightBracket):
            return "RightBracket";
            break;
        case (keycodes::keyGraveAccent):
            return "GraveAccent";
            break;
        case (keycodes::keyWorld1):
            return "World1";
            break;
        case (keycodes::keyWorld2):
            return "World2";
            break;
        case (keycodes::keyEscape):
            return "Escape";
            break;
        case (keycodes::keyEnter):
            return "Enter";
            break;
        case (keycodes::keyTab):
            return "Tab";
            break;
        case (keycodes::keyBackspace):
            return "Backspace";
            break;
        case (keycodes::keyInsert):
            return "Insert";
            break;
        case (keycodes::keyDelete):
            return "Delete";
            break;
        case (keycodes::keyRight):
            return "Right";
            break;
        case (keycodes::keyLeft):
            return "Left";
            break;
        case (keycodes::keyDown):
            return "Down";
            break;
        case (keycodes::keyUp):
            return "Up";
            break;
        case (keycodes::keyPageUp):
            return "PageUp";
            break;
        case (keycodes::keyPageDown):
            return "PageDown";
            break;
        case (keycodes::keyHome):
            return "Home";
            break;
        case (keycodes::keyEnd):
            return "End";
            break;
        case (keycodes::keyCapsLock):
            return "CapsLock";
            break;
        case (keycodes::keyScrollLock):
            return "ScrollLock";
            break;
        case (keycodes::keyNumLock):
            return "NumLock";
            break;
        case (keycodes::keyPrintScreen):
            return "PrintScreen";
            break;
        case (keycodes::keyPause):
            return "Pause";
            break;
        case (keycodes::keyF1):
            return "F1";
            break;
        case (keycodes::keyF2):
            return "F2";
            break;
        case (keycodes::keyF3):
            return "F3";
            break;
        case (keycodes::keyF4):
            return "F4";
            break;
        case (keycodes::keyF5):
            return "F5";
            break;
        case (keycodes::keyF6):
            return "F6";
            break;
        case (keycodes::keyF7):
            return "F7";
            break;
        case (keycodes::keyF8):
            return "F8";
            break;
        case (keycodes::keyF9):
            return "F9";
            break;
        case (keycodes::keyF10):
            return "F10";
            break;
        case (keycodes::keyF11):
            return "F11";
            break;
        case (keycodes::keyF12):
            return "F12";
            break;
        case (keycodes::keyF13):
            return "F13";
            break;
        case (keycodes::keyF14):
            return "F14";
            break;
        case (keycodes::keyF15):
            return "F15";
            break;
        case (keycodes::keyF16):
            return "F16";
            break;
        case (keycodes::keyF17):
            return "F17";
            break;
        case (keycodes::keyF18):
            return "F18";
            break;
        case (keycodes::keyF19):
            return "F19";
            break;
        case (keycodes::keyF20):
            return "F20";
            break;
        case (keycodes::keyF21):
            return "F21";
            break;
        case (keycodes::keyF22):
            return "F22";
            break;
        case (keycodes::keyF23):
            return "F23";
            break;
        case (keycodes::keyF24):
            return "F24";
            break;
        case (keycodes::keyF25):
            return "F25";
            break;
        case (keycodes::keyKp0):
            return "Kp0";
            break;
        case (keycodes::keyKp1):
            return "Kp1";
            break;
        case (keycodes::keyKp2):
            return "Kp2";
            break;
        case (keycodes::keyKp3):
            return "Kp3";
            break;
        case (keycodes::keyKp4):
            return "Kp4";
            break;
        case (keycodes::keyKp5):
            return "Kp5";
            break;
        case (keycodes::keyKp6):
            return "Kp6";
            break;
        case (keycodes::keyKp7):
            return "Kp7";
            break;
        case (keycodes::keyKp8):
            return "Kp8";
            break;
        case (keycodes::keyKp9):
            return "Kp9";
            break;
        case (keycodes::keyKpDecimal):
            return "KpDecimal";
            break;
        case (keycodes::keyKpDivide):
            return "KpDivide";
            break;
        case (keycodes::keyKpMultiply):
            return "KpMultiply";
            break;
        case (keycodes::keyKpSubtract):
            return "KpSubtract";
            break;
        case (keycodes::keyKpAdd):
            return "KpAdd";
            break;
        case (keycodes::keyKpEnter):
            return "KpEnter";
            break;
        case (keycodes::keyKpEqual):
            return "KpEqual";
            break;
        case (keycodes::keyLeftShift):
            return "LeftShift";
            break;
        case (keycodes::keyLeftControl):
            return "LeftControl";
            break;
        case (keycodes::keyLeftAlt):
            return "LeftAlt";
            break;
        case (keycodes::keyLeftSuper):
            return "LeftSuper";
            break;
        case (keycodes::keyRightShift):
            return "RightShift";
            break;
        case (keycodes::keyRightControl):
            return "RightControl";
            break;
        case (keycodes::keyRightAlt):
            return "RightAlt";
            break;
        case (keycodes::keyRightSuper):
            return "RightSuper";
            break;
        case (keycodes::keyMenu):
            return "Menu";
            break;
        default:
            return "unknown key";
            break;
    }
}