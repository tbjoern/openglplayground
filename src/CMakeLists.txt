set(target playground)

set(sources
    main.cpp
)

set(includes
    vertex_data.hpp
)

set(libraries
    OpenGL
    glfw
    GLEW
    SOIL
)

add_executable(${target} ${sources} ${includes})
target_link_libraries(${target} PUBLIC ${libraries})
