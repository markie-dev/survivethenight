/// \file LMouse.cpp
/// \brief Code for the mouse handler class LMouse.

#include "Mouse.h"

/// Constructor initializes all mouse states to zero.
LMouse::LMouse() {
    ZeroMemory(&m_CursorPosition, sizeof(POINT));
    ZeroMemory(&m_PrevCursorPosition, sizeof(POINT));
    ZeroMemory(m_ButtonState, sizeof(m_ButtonState));
    ZeroMemory(m_OldButtonState, sizeof(m_OldButtonState));
}

/// Use the Windows API functions to get the state of the mouse and its position.
/// The results are stored in m_ButtonState and m_CursorPosition.
void LMouse::GetState() {
    CopyMemory(m_OldButtonState, m_ButtonState, sizeof(m_ButtonState)); //copy to old state
    CopyMemory(&m_PrevCursorPosition, &m_CursorPosition, sizeof(POINT)); //copy previous cursor position

    m_ButtonState[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 0x80 : 0x00;
    m_ButtonState[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 0x80 : 0x00;
    m_ButtonState[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ? 0x80 : 0x00;

    GetCursorPos(&m_CursorPosition);
    ScreenToClient(m_Hwnd, &m_CursorPosition); //assuming m_hWnd is the handle to the game window
}

/// Retrieve the current mouse position.
const POINT& LMouse::GetPosition() const {
    return m_CursorPosition;
}

/// Get the difference in position since the last poll.
const POINT& LMouse::GetDeltaPosition() const {
    static POINT delta;
    delta.x = m_CursorPosition.x - m_PrevCursorPosition.x;
    delta.y = m_CursorPosition.y - m_PrevCursorPosition.y;
    return delta;
}

/// Check if a mouse button is pressed.
bool LMouse::ButtonDown(int button) const {
    if (button < 0 || button >= 3) return false; //button index out of range
    return (m_ButtonState[button] & 0x80) == 0x80;
}

/// Check if a button changed from up to down.
bool LMouse::ButtonTriggerDown(int button) {
    return ButtonTrigger(button, true);
}

/// Check if a button changed from down to up.
bool LMouse::ButtonTriggerUp(int button) {
    return ButtonTrigger(button, false);
}

/// Check for button changing state.
bool LMouse::ButtonTrigger(int button, bool bDown) {
    if (button < 0 || button >= 3) return false; //button index out of range
    const bool bIsDown = (m_ButtonState[button] & 0x80) == 0x80;
    const bool bStateChanged = m_ButtonState[button] != m_OldButtonState[button];
    return bStateChanged && (bIsDown == bDown);
}