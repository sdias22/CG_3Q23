project(memory)
add_executable(${PROJECT_NAME} main.cpp window.cpp camera.cpp ground.cpp cube.cpp select.cpp)
enable_abcg(${PROJECT_NAME})
