cmake_minimum_required(VERSION 3.20)
project(sand)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#GLAD
add_library(glad STATIC
    src/vendor/glad/src/glad.c
)
target_include_directories(glad PUBLIC src/vendor/glad/include)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

#GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)


# STB IMAGE
add_library(stb_image STATIC
  src/vendor/stb_image/stb_image.cpp
)
target_include_directories(stb_image PUBLIC src/vendor/stb_image)
set_target_properties(stb_image PROPERTIES POSITION_INDEPENDENT_CODE ON)

#GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

add_executable(sand 
    src/Sand.cpp
    src/Renderer.cpp
    src/Buffers/IndexBuffer.cpp
    src/Buffers/VertexArray.cpp
    src/Buffers/VertexBuffer.cpp
    src/Shader.cpp
)


target_link_libraries(sand PUBLIC
    glad
    glfw
    stb_image
    glm::glm
)
