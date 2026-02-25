#include "gl_loader.h"
#include <tgl/core/log.h>

// Define function pointers
#define X(ret, name, args) PFN_##name tgl_##name = nullptr;
TGL_GL_FUNCTIONS
#undef X

// Platform-specific GL function loading
#ifdef TGL_PLATFORM_WINDOWS
#include <windows.h>
static void* get_gl_proc(const char* name) {
    void* p = (void*)wglGetProcAddress(name);
    if (!p || p == (void*)0x1 || p == (void*)0x2 || p == (void*)0x3 || p == (void*)-1) {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void*)GetProcAddress(module, name);
    }
    return p;
}
#elif defined(TGL_PLATFORM_MACOS)
#include <dlfcn.h>
static void* get_gl_proc(const char* name) {
    static void* lib = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
    return lib ? dlsym(lib, name) : nullptr;
}
#else // Linux
#include <dlfcn.h>
typedef void (*GLXextFuncPtr)(void);
extern "C" GLXextFuncPtr glXGetProcAddressARB(const unsigned char*);
static void* get_gl_proc(const char* name) {
    void* p = (void*)glXGetProcAddressARB((const unsigned char*)name);
    if (!p) {
        static void* lib = dlopen("libGL.so.1", RTLD_LAZY);
        if (lib) p = dlsym(lib, name);
    }
    return p;
}
#endif

namespace tgl {

bool load_gl_functions() {
    bool all_ok = true;

    #define X(ret, name, args) \
        tgl_##name = (PFN_##name)get_gl_proc(#name); \
        if (!tgl_##name) { \
            log::warn("Failed to load GL function: %s", #name); \
            all_ok = false; \
        }
    TGL_GL_FUNCTIONS
    #undef X

    if (all_ok) {
        log::info("All OpenGL 3.3 functions loaded successfully");
    }
    return all_ok;
}

} // namespace tgl
