#ifndef TGL_INPUT_INPUT_H
#define TGL_INPUT_INPUT_H

#include "keyboard.h"
#include "mouse.h"
#include "../math/vec2.h"

namespace tgl {

class Input {
public:
    Input() { reset(); }

    // Called by the window system
    void on_key(int key, int action);
    void on_mouse_button(int button, int action);
    void on_mouse_move(double x, double y);
    void on_scroll(double x, double y);

    // Called once per frame (swaps current/previous states)
    void update();
    void reset();

    // Keyboard queries
    bool key_down(Key key) const;       // held this frame
    bool key_pressed(Key key) const;    // just pressed this frame
    bool key_released(Key key) const;   // just released this frame

    // Mouse queries
    bool mouse_down(MouseButton btn) const;
    bool mouse_pressed(MouseButton btn) const;
    bool mouse_released(MouseButton btn) const;

    Vec2 mouse_position() const { return mouse_pos_; }
    Vec2 mouse_delta() const { return mouse_pos_ - prev_mouse_pos_; }
    Vec2 scroll_delta() const { return scroll_delta_; }

private:
    static constexpr int MAX_KEYS = static_cast<int>(Key::MAX_KEYS);
    static constexpr int MAX_BUTTONS = static_cast<int>(MouseButton::MAX_BUTTONS);

    bool keys_[MAX_KEYS] = {};
    bool prev_keys_[MAX_KEYS] = {};
    bool buttons_[MAX_BUTTONS] = {};
    bool prev_buttons_[MAX_BUTTONS] = {};
    Vec2 mouse_pos_;
    Vec2 prev_mouse_pos_;
    Vec2 scroll_delta_;
};

} // namespace tgl

#endif // TGL_INPUT_INPUT_H
