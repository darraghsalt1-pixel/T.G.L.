#include <tgl/input/input.h>
#include <cstring>

namespace tgl {

void Input::reset() {
    std::memset(keys_, 0, sizeof(keys_));
    std::memset(prev_keys_, 0, sizeof(prev_keys_));
    std::memset(buttons_, 0, sizeof(buttons_));
    std::memset(prev_buttons_, 0, sizeof(prev_buttons_));
    mouse_pos_ = prev_mouse_pos_ = Vec2::zero();
    scroll_delta_ = Vec2::zero();
}

void Input::update() {
    std::memcpy(prev_keys_, keys_, sizeof(keys_));
    std::memcpy(prev_buttons_, buttons_, sizeof(buttons_));
    prev_mouse_pos_ = mouse_pos_;
    scroll_delta_ = Vec2::zero();
}

void Input::on_key(int key, int action) {
    if (key >= 0 && key < MAX_KEYS) {
        keys_[key] = (action != 0);
    }
}

void Input::on_mouse_button(int button, int action) {
    if (button >= 0 && button < MAX_BUTTONS) {
        buttons_[button] = (action != 0);
    }
}

void Input::on_mouse_move(double x, double y) {
    mouse_pos_ = {(float)x, (float)y};
}

void Input::on_scroll(double x, double y) {
    scroll_delta_ = {(float)x, (float)y};
}

bool Input::key_down(Key key) const {
    int k = static_cast<int>(key);
    return (k >= 0 && k < MAX_KEYS) ? keys_[k] : false;
}

bool Input::key_pressed(Key key) const {
    int k = static_cast<int>(key);
    return (k >= 0 && k < MAX_KEYS) ? (keys_[k] && !prev_keys_[k]) : false;
}

bool Input::key_released(Key key) const {
    int k = static_cast<int>(key);
    return (k >= 0 && k < MAX_KEYS) ? (!keys_[k] && prev_keys_[k]) : false;
}

bool Input::mouse_down(MouseButton btn) const {
    int b = static_cast<int>(btn);
    return (b >= 0 && b < MAX_BUTTONS) ? buttons_[b] : false;
}

bool Input::mouse_pressed(MouseButton btn) const {
    int b = static_cast<int>(btn);
    return (b >= 0 && b < MAX_BUTTONS) ? (buttons_[b] && !prev_buttons_[b]) : false;
}

bool Input::mouse_released(MouseButton btn) const {
    int b = static_cast<int>(btn);
    return (b >= 0 && b < MAX_BUTTONS) ? (!buttons_[b] && prev_buttons_[b]) : false;
}

} // namespace tgl
