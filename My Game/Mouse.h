/// \file Mouse.h
/// \brief Interface for the mouse handler class LMouse.

#ifndef __L4RC_ENGINE_MOUSE_H__
#define __L4RC_ENGINE_MOUSE_H__

#include "WindowDesc.h"

/// \brief The mouse handler.
///
/// This is a polled mouse handler. Its GetState() function
/// must be called once per frame. The mouse state from
/// the previous frame is retained so that queries can determine
/// whether a mouse button changed state or the mouse was moved.

class LMouse : public LWindowDesc {
private:
    POINT m_CursorPosition;           ///< Current position of the cursor.
    POINT m_PrevCursorPosition;       ///< Previous position of the cursor.
    unsigned char m_ButtonState[3];   ///< Current state of the mouse buttons.
    unsigned char m_OldButtonState[3];///< Previous state of the mouse buttons.

    bool ButtonTrigger(int button, bool bDown); ///< Check for button changing state.

public:
    LMouse(); ///< Constructor.

    void GetState(); ///< Poll the mouse state.

    const POINT& GetPosition() const; ///< Retrieve the current mouse position.
    const POINT& GetDeltaPosition() const; ///< Get the difference in position since the last poll.

    bool ButtonDown(int button) const; ///< Check if a mouse button is pressed.
    bool ButtonTriggerDown(int button); ///< Check if a button changed from up to down.
    bool ButtonTriggerUp(int button); ///< Check if a button changed from down to up.
}; //LMouse

#endif //__L4RC_ENGINE_MOUSE_H_