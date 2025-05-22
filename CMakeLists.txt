cmake_minimum_required(VERSION 3.15)
project(A-compleatly-normal-fishing-game VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE "GLVND")

# Packages
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED SDL2_image)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)


include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

link_directories(
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${GLEW_LIBRARY_DIRS}
)

# subdirectories
add_subdirectory( src/engine )
add_subdirectory( src/game )

include_directories(${Boost_INCLUDE_DIRS})

# Executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}       # SDL2 linking
    ${SDL2_IMAGE_LIBRARIES} # SDL images
    OpenGL::GL              # Linking OpenGL
    ${GLEW_LIBRARIES}       # GLEW linking
    ${Boost_LIBRARIES}   # Boost linking
    engine
    game
)

# Copy assets (e.g. shaders)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "---- Copying assets to output directory ----"
)