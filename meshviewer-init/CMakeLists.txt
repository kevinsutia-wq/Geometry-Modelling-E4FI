cmake_minimum_required(VERSION 3.10)
project(meshviewer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -w")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(X11 REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/myproj)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/myproj/include)
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${X11_INCLUDE_DIR})

# Source files
set(SOURCES
    myproj/main.cpp
    myproj/myMesh.cpp
    myproj/myPoint3D.cpp
    myproj/myVector3D.cpp
    myproj/myVertex.cpp
    myproj/myHalfedge.cpp
    myproj/myFace.cpp
)

# Create executable
add_executable(meshviewer ${SOURCES})

# Link libraries
target_link_libraries(meshviewer 
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    $ENV{HOME}/.local/lib/libglut.a
    ${X11_LIBRARIES}
    m
    pthread
    Xrandr
    Xxf86vm
    Xi
)

# Set working directory to where shaders and meshes are
set_target_properties(meshviewer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/myproj")
