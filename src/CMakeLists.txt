set(target playground)

set(sources
    main.cpp
)

set(includes

)

set(libraries
    OpenGL
    glfw
    GLEW
    SOIL
)

add_executable(${target} ${sources} ${includes})
target_link_libraries(${target} PUBLIC ${libraries})
