cmake_minimum_required(VERSION 3.10)
project(Model3DLoader VERSION 1.0)

# GLAD jako biblioteka
add_library(glad STATIC Model3DLoader/glad/src/glad.c)
target_include_directories(glad PUBLIC Model3DLoader/glad/include)

# Aplikacja
add_executable(${PROJECT_NAME}
    Model3DLoader/main.cpp
)

target_sources(${PROJECT_NAME} PRIVATE
    Model3DLoader/window.hpp
)

# Standard C++
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# Biblioteki i include'y
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/glm)

# VS debugger path
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Model3DLoader"
)