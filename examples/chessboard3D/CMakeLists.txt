project(chessboard3D)
add_executable(${PROJECT_NAME} main.cpp window.cpp camera.cpp ground.cpp pieces.cpp)
enable_abcg(${PROJECT_NAME})
