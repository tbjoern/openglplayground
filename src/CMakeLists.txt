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
)

add_executable(${target} ${sources} ${includes})
target_link_libraries(${target} PUBLIC ${libraries})

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
