#include <QObject>

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>

#include "keystate.h"

bool isPress(KeyEvents key) {
    unsigned char keyMap[16] = {0};
    GetKeys(reinterpret_cast<BigEndianUInt32*>(&keyMap));
    int inKeyCode = 0;

    switch (key) {
    case KeyEvents::KeyDown:
        inKeyCode = kVK_DownArrow;
        break;
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
#endif
