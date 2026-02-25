/*
 * T.G.L. - The Graphics Library
 * A lightweight, easy-to-use C++ graphics library for games and simulations.
 *
 * Single-header include for the entire library.
 */

#ifndef TGL_H
#define TGL_H

// Core
#include "core/types.h"
#include "core/app.h"
#include "core/timer.h"
#include "core/log.h"

// Math
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include "math/transform.h"
#include "math/math_utils.h"

// Platform
#include "platform/window.h"

// Graphics
#include "graphics/color.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/vertex.h"
#include "graphics/mesh.h"
#include "graphics/renderer.h"
#include "graphics/sprite.h"
#include "graphics/camera.h"
#include "graphics/font.h"

// Input
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/input.h"

// Audio
#include "audio/audio.h"

// ECS
#include "ecs/ecs.h"

// Utils
#include "utils/file_utils.h"
#include "utils/image_loader.h"

#endif // TGL_H
