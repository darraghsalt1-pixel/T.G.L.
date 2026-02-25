# T.G.L. Examples

# Example 1: Hello Triangle
add_executable(hello_triangle hello_triangle.cpp)
target_link_libraries(hello_triangle tgl)

# Example 2: Spinning Cube (3D)
add_executable(spinning_cube spinning_cube.cpp)
target_link_libraries(spinning_cube tgl)

# Example 3: Sprite Batch (2D)
add_executable(sprite_demo sprite_demo.cpp)
target_link_libraries(sprite_demo tgl)

# Example 4: ECS Demo
add_executable(ecs_demo ecs_demo.cpp)
target_link_libraries(ecs_demo tgl)

# Example 5: FPS Camera
add_executable(fps_camera fps_camera.cpp)
target_link_libraries(fps_camera tgl)
