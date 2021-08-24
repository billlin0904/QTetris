#include <qsystemdetection.h>

#include "keystate.h"

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>

bool isPress(KeyEvents key) noexcept {
    unsigned char keyMap[16] = {0};
    GetKeys(reinterpret_cast<BigEndianUInt32*>(&keyMap));
    int inKeyCode = 0;

    switch (key) {
    case KeyEvents::KeyRotate:
        inKeyCode = kVK_Space;
        break;
    case KeyEvents::KeyAntiRotate:
        inKeyCode = kVK_Option;
        break;
    default:
        return false;
    }

    return (0 != ((keyMap[ inKeyCode >> 3] >> (inKeyCode & 7)) & 1));
}
#else
#include <Windows.h>

bool isPress(KeyEvents key) noexcept {
    SHORT state = 0;

    switch (key) {
    case KeyEvents::KeyRotate:
        state = GetKeyState(VK_SPACE);
        break;
    case KeyEvents::KeyAntiRotate:
        state = GetKeyState(VK_MENU);
        break;
    default:
        return false;
    }

    return state & 0x8000;
}
#endif
