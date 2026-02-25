#include <tgl/platform/window.h>
#include <tgl/core/log.h>
#include "../graphics/gl_loader.h"

namespace tgl {

// ============================================================
// Platform-specific window implementations
// ============================================================

#ifdef TGL_PLATFORM_WINDOWS
// ------ Windows (Win32 + WGL) ------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
typedef BOOL  (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

struct Window::PlatformData {
    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HGLRC hglrc = nullptr;
    Window* self = nullptr;
};

static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window::PlatformData* pd = (Window::PlatformData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!pd) return DefWindowProc(hwnd, msg, wp, lp);
    Window* w = pd->self;

    switch (msg) {
    case WM_CLOSE:
        w->set_should_close(true);
        return 0;
    case WM_SIZE: {
        int width = LOWORD(lp), height = HIWORD(lp);
        if (w->on_resize) w->on_resize(width, height);
        return 0;
    }
    case WM_KEYDOWN: case WM_KEYUP:
        if (w->on_key) w->on_key((int)wp, 0, msg == WM_KEYDOWN ? 1 : 0);
        return 0;
    case WM_MOUSEMOVE:
        if (w->on_mouse_move) w->on_mouse_move(LOWORD(lp), HIWORD(lp));
        return 0;
    case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        if (w->on_mouse_button) w->on_mouse_button(0, msg == WM_LBUTTONDOWN ? 1 : 0);
        return 0;
    case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        if (w->on_mouse_button) w->on_mouse_button(1, msg == WM_RBUTTONDOWN ? 1 : 0);
        return 0;
    case WM_MBUTTONDOWN: case WM_MBUTTONUP:
        if (w->on_mouse_button) w->on_mouse_button(2, msg == WM_MBUTTONDOWN ? 1 : 0);
        return 0;
    case WM_MOUSEWHEEL:
        if (w->on_scroll) w->on_scroll(0, GET_WHEEL_DELTA_WPARAM(wp) / 120.0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

Result Window::create(const WindowConfig& config) {
    width_ = config.width;
    height_ = config.height;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "TGL_WND";
    RegisterClassEx(&wc);

    RECT rect = {0, 0, config.width, config.height};
    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!config.resizable) style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    AdjustWindowRect(&rect, style, FALSE);

    platform_ = new PlatformData();
    platform_->self = this;
    platform_->hwnd = CreateWindowEx(0, "TGL_WND", config.title.c_str(), style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!platform_->hwnd) {
        return {ErrorCode::WindowCreationFailed, "Failed to create Win32 window"};
    }

    SetWindowLongPtr(platform_->hwnd, GWLP_USERDATA, (LONG_PTR)platform_);
    native_handle_ = platform_->hwnd;

    platform_->hdc = GetDC(platform_->hwnd);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    int pf = ChoosePixelFormat(platform_->hdc, &pfd);
    SetPixelFormat(platform_->hdc, pf, &pfd);

    HGLRC temp_ctx = wglCreateContext(platform_->hdc);
    wglMakeCurrent(platform_->hdc, temp_ctx);

    auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB) {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        platform_->hglrc = wglCreateContextAttribsARB(platform_->hdc, nullptr, attribs);
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(temp_ctx);
        wglMakeCurrent(platform_->hdc, platform_->hglrc);
    } else {
        platform_->hglrc = temp_ctx;
    }

    gl_context_ = platform_->hglrc;
    load_gl_functions();

    auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(config.vsync ? 1 : 0);

    ShowWindow(platform_->hwnd, SW_SHOW);
    log::info("Window created: %dx%d (Win32)", width_, height_);
    return {};
}

void Window::destroy() {
    if (platform_) {
        if (platform_->hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(platform_->hglrc);
        }
        if (platform_->hwnd) {
            DestroyWindow(platform_->hwnd);
        }
        delete platform_;
        platform_ = nullptr;
    }
}

void Window::poll_events() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::swap_buffers() {
    if (platform_ && platform_->hdc) SwapBuffers(platform_->hdc);
}

void Window::set_title(const std::string& title) {
    if (platform_ && platform_->hwnd) SetWindowText(platform_->hwnd, title.c_str());
}

void Window::set_vsync(bool enabled) {
    auto fn = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (fn) fn(enabled ? 1 : 0);
}

void Window::set_fullscreen(bool enabled) {
    // Simplified fullscreen toggle
    if (!platform_) return;
    DWORD style = enabled ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    SetWindowLong(platform_->hwnd, GWL_STYLE, style);
    if (enabled) {
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(MonitorFromWindow(platform_->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
        SetWindowPos(platform_->hwnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_FRAMECHANGED);
    }
    ShowWindow(platform_->hwnd, SW_SHOW);
}

void Window::set_cursor_visible(bool visible) {
    ShowCursor(visible ? TRUE : FALSE);
}

void Window::set_cursor_locked(bool locked) {
    if (locked && platform_ && platform_->hwnd) {
        RECT rect;
        GetClientRect(platform_->hwnd, &rect);
        POINT tl = {rect.left, rect.top}, br = {rect.right, rect.bottom};
        ClientToScreen(platform_->hwnd, &tl);
        ClientToScreen(platform_->hwnd, &br);
        RECT clip = {tl.x, tl.y, br.x, br.y};
        ClipCursor(&clip);
    } else {
        ClipCursor(nullptr);
    }
}

#elif defined(TGL_PLATFORM_LINUX) && !defined(TGL_USE_WAYLAND)
// ------ Linux (X11 + GLX) ------

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/glx.h>

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

struct Window::PlatformData {
    Display* display = nullptr;
    ::Window xwindow = 0;
    GLXContext glx_ctx = nullptr;
    Atom wm_delete;
    Colormap colormap = 0;
    Window* self = nullptr;
};

// Map X11 keysym to TGL key code
static int x11_to_tgl_key(KeySym ks) {
    if (ks >= XK_a && ks <= XK_z) return 65 + (ks - XK_a);
    if (ks >= XK_A && ks <= XK_Z) return 65 + (ks - XK_A);
    if (ks >= XK_0 && ks <= XK_9) return 48 + (ks - XK_0);
    if (ks >= XK_F1 && ks <= XK_F12) return 290 + (ks - XK_F1);
    switch (ks) {
        case XK_Escape:    return 256;
        case XK_Return:    return 257;
        case XK_Tab:       return 258;
        case XK_BackSpace: return 259;
        case XK_space:     return 32;
        case XK_Left:      return 263;
        case XK_Right:     return 262;
        case XK_Up:        return 265;
        case XK_Down:      return 264;
        case XK_Shift_L:   return 340;
        case XK_Shift_R:   return 344;
        case XK_Control_L: return 341;
        case XK_Control_R: return 345;
        default:           return -1;
    }
}

Result Window::create(const WindowConfig& config) {
    width_ = config.width;
    height_ = config.height;

    platform_ = new PlatformData();
    platform_->self = this;
    platform_->display = XOpenDisplay(nullptr);
    if (!platform_->display) {
        delete platform_; platform_ = nullptr;
        return {ErrorCode::WindowCreationFailed, "Failed to open X11 display"};
    }

    int screen = DefaultScreen(platform_->display);

    // Choose framebuffer config
    int fb_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int fb_count;
    GLXFBConfig* fbc = glXChooseFBConfig(platform_->display, screen, fb_attribs, &fb_count);
    if (!fbc || fb_count == 0) {
        XCloseDisplay(platform_->display);
        delete platform_; platform_ = nullptr;
        return {ErrorCode::WindowCreationFailed, "No suitable framebuffer config"};
    }

    GLXFBConfig best_fbc = fbc[0];
    XVisualInfo* vi = glXGetVisualFromFBConfig(platform_->display, best_fbc);
    XFree(fbc);

    platform_->colormap = XCreateColormap(platform_->display, RootWindow(platform_->display, screen), vi->visual, AllocNone);

    XSetWindowAttributes swa = {};
    swa.colormap = platform_->colormap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     StructureNotifyMask;

    platform_->xwindow = XCreateWindow(
        platform_->display, RootWindow(platform_->display, screen),
        0, 0, config.width, config.height, 0,
        vi->depth, InputOutput, vi->visual,
        CWColormap | CWEventMask, &swa);

    XFree(vi);

    if (!platform_->xwindow) {
        XCloseDisplay(platform_->display);
        delete platform_; platform_ = nullptr;
        return {ErrorCode::WindowCreationFailed, "Failed to create X11 window"};
    }

    XStoreName(platform_->display, platform_->xwindow, config.title.c_str());
    platform_->wm_delete = XInternAtom(platform_->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(platform_->display, platform_->xwindow, &platform_->wm_delete, 1);
    XMapWindow(platform_->display, platform_->xwindow);

    native_handle_ = (void*)(uintptr_t)platform_->xwindow;

    // Create OpenGL 3.3 context
    auto glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
        glXGetProcAddressARB((const unsigned char*)"glXCreateContextAttribsARB");

    if (glXCreateContextAttribsARB) {
        int ctx_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };
        platform_->glx_ctx = glXCreateContextAttribsARB(platform_->display, best_fbc, nullptr, True, ctx_attribs);
    } else {
        platform_->glx_ctx = glXCreateNewContext(platform_->display, best_fbc, GLX_RGBA_TYPE, nullptr, True);
    }

    if (!platform_->glx_ctx) {
        XDestroyWindow(platform_->display, platform_->xwindow);
        XCloseDisplay(platform_->display);
        delete platform_; platform_ = nullptr;
        return {ErrorCode::OpenGLInitFailed, "Failed to create GLX context"};
    }

    glXMakeCurrent(platform_->display, platform_->xwindow, platform_->glx_ctx);
    gl_context_ = platform_->glx_ctx;
    load_gl_functions();

    // VSync
    typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int);
    auto glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const unsigned char*)"glXSwapIntervalEXT");
    if (glXSwapIntervalEXT) {
        glXSwapIntervalEXT(platform_->display, platform_->xwindow, config.vsync ? 1 : 0);
    }

    log::info("Window created: %dx%d (X11/GLX)", width_, height_);
    return {};
}

void Window::destroy() {
    if (platform_) {
        if (platform_->glx_ctx) {
            glXMakeCurrent(platform_->display, None, nullptr);
            glXDestroyContext(platform_->display, platform_->glx_ctx);
        }
        if (platform_->xwindow) {
            XDestroyWindow(platform_->display, platform_->xwindow);
        }
        if (platform_->colormap) {
            XFreeColormap(platform_->display, platform_->colormap);
        }
        if (platform_->display) {
            XCloseDisplay(platform_->display);
        }
        delete platform_;
        platform_ = nullptr;
    }
}

void Window::poll_events() {
    if (!platform_ || !platform_->display) return;
    XEvent ev;
    while (XPending(platform_->display)) {
        XNextEvent(platform_->display, &ev);
        switch (ev.type) {
        case ClientMessage:
            if ((Atom)ev.xclient.data.l[0] == platform_->wm_delete)
                should_close_ = true;
            break;
        case ConfigureNotify:
            if (ev.xconfigure.width != width_ || ev.xconfigure.height != height_) {
                width_ = ev.xconfigure.width;
                height_ = ev.xconfigure.height;
                if (on_resize) on_resize(width_, height_);
            }
            break;
        case KeyPress: case KeyRelease: {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            int key = x11_to_tgl_key(ks);
            if (on_key) on_key(key, 0, ev.type == KeyPress ? 1 : 0);
            break;
        }
        case ButtonPress: case ButtonRelease: {
            int btn = ev.xbutton.button - 1; // X11 buttons are 1-indexed
            if (btn == 3) { // scroll up
                if (on_scroll) on_scroll(0, 1);
            } else if (btn == 4) { // scroll down
                if (on_scroll) on_scroll(0, -1);
            } else {
                if (on_mouse_button) on_mouse_button(btn, ev.type == ButtonPress ? 1 : 0);
            }
            break;
        }
        case MotionNotify:
            if (on_mouse_move) on_mouse_move(ev.xmotion.x, ev.xmotion.y);
            break;
        }
    }
}

void Window::swap_buffers() {
    if (platform_ && platform_->display) {
        glXSwapBuffers(platform_->display, platform_->xwindow);
    }
}

void Window::set_title(const std::string& title) {
    if (platform_ && platform_->display) {
        XStoreName(platform_->display, platform_->xwindow, title.c_str());
    }
}

void Window::set_vsync(bool enabled) {
    if (!platform_ || !platform_->display) return;
    typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int);
    auto fn = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const unsigned char*)"glXSwapIntervalEXT");
    if (fn) fn(platform_->display, platform_->xwindow, enabled ? 1 : 0);
}

void Window::set_fullscreen(bool /*enabled*/) {
    // TODO: implement via _NET_WM_STATE_FULLSCREEN
    log::warn("Fullscreen toggle not yet implemented on X11");
}

void Window::set_cursor_visible(bool visible) {
    if (!platform_ || !platform_->display) return;
    if (!visible) {
        Cursor cur;
        Pixmap blank = XCreateBitmapFromData(platform_->display, platform_->xwindow, "\0", 1, 1);
        XColor dummy = {};
        cur = XCreatePixmapCursor(platform_->display, blank, blank, &dummy, &dummy, 0, 0);
        XDefineCursor(platform_->display, platform_->xwindow, cur);
        XFreeCursor(platform_->display, cur);
        XFreePixmap(platform_->display, blank);
    } else {
        XUndefineCursor(platform_->display, platform_->xwindow);
    }
}

void Window::set_cursor_locked(bool /*locked*/) {
    log::warn("Cursor lock not yet implemented on X11");
}

#else
// ------ Stub (unsupported platform or Wayland placeholder) ------

struct Window::PlatformData {};

Result Window::create(const WindowConfig&) {
    return {ErrorCode::WindowCreationFailed, "Platform not supported yet"};
}
void Window::destroy() {}
void Window::poll_events() {}
void Window::swap_buffers() {}
void Window::set_title(const std::string&) {}
void Window::set_vsync(bool) {}
void Window::set_fullscreen(bool) {}
void Window::set_cursor_visible(bool) {}
void Window::set_cursor_locked(bool) {}

#endif

// Common code
Window::~Window() { destroy(); }
bool Window::should_close() const { return should_close_; }
void Window::set_should_close(bool close) { should_close_ = close; }

} // namespace tgl
