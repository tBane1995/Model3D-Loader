cmake_minimum_required(VERSION 3.10)
project(Model3DLoader VERSION 1.0)

add_library(glad STATIC Model3DLoader/glad/src/glad.c)
target_include_directories(glad PUBLIC Model3DLoader/glad/include)

# main
add_executable(${PROJECT_NAME}
    Model3DLoader/main.cpp
)

# headers
target_sources(${PROJECT_NAME} PRIVATE
    Model3DLoader/WideUTF8Converter.hpp
    Model3DLoader/Time.hpp
    Model3DLoader/Camera.hpp
    Model3DLoader/Transform.hpp
    Model3DLoader/stb_image.h
    Model3DLoader/Textures.hpp
    Model3DLoader/Shaders.hpp
    Model3DLoader/Programs.hpp
    Model3DLoader/Materials.hpp
    Model3DLoader/Model3D.hpp
)

# Standard C++
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# GLFW (binary version)
target_include_directories(${PROJECT_NAME} PRIVATE C:/glfw-3.4.bin.WIN64/include)
target_link_directories(${PROJECT_NAME} PRIVATE C:/glfw-3.4.bin.WIN64/lib-vc2022)

#  GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

# GLAD
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

# GLM
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/glm)

# VS debugger path
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Model3DLoader"
)