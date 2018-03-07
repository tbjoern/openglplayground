#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <chrono>

#include <cmath>

namespace {

    class ShaderCompilationException : public std::exception {
    private:
        std::string _what_message;
        GLuint _shader;
        bool _shader_set;
    public:
        ShaderCompilationException(std::string shader_source_filename, std::string additional_info="") : _what_message(),  _shader(), _shader_set(true) {
            std::stringstream s;
            s << "Shader compilation failed: " << shader_source_filename << std::endl << additional_info;
            _what_message = s.str();
        };

        ShaderCompilationException(std::string shader_source_filename, GLuint shader, std::string additional_info="") : _what_message(),  _shader(shader), _shader_set(true) {
            std::stringstream s;
            s << "Shader compilation failed: " << shader_source_filename << std::endl << additional_info;
            _what_message = s.str();
        };

        virtual ~ShaderCompilationException() = default;

        const char* what() const noexcept override {
            return _what_message.c_str();
        }

        bool hasShader() const {
            return _shader_set;
        }

        GLuint shader() const {
            return _shader;
        }
    };

    // https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c
    std::string readFile(const std::string& fileName)
    {
        std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        ifs.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

        std::ifstream::pos_type fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }

    GLuint compileShaderFromSourceFile(const std::string& filename, GLenum shader_type) {
        std::string shader_source;
        try {
           shader_source = readFile(filename);
        } catch (const std::ifstream::failure& e) {
            throw ShaderCompilationException(filename, "Could not read file");
        }
        GLuint shader = glCreateShader(shader_type);
        const char* source_cstyle = shader_source.c_str();
        glShaderSource(shader, 1, &source_cstyle, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_TRUE) {
            return shader;
        } else {
            throw ShaderCompilationException(filename, shader ,"Compilation Error");
        }
    }

    std::string getShaderCompileLog(GLuint shader) {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        return std::string(buffer);
    }

    GLuint loadAndBindTextureFromImageFile(const std::string& filename, GLenum active_texture) {
        GLuint texture;
        glGenTextures(1, &texture);

        glActiveTexture(active_texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(image);

        return texture;
    }
}

int main() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    // load shaders
    std::string vertex_shader_source_file = "shaders/vertex_shader.glsl";
    std::string fragment_shader_source_file = "shaders/fragment_shader.glsl";
    
    // compile shaders
    GLuint vertex_shader;
    GLuint fragment_shader;

    try {
        vertex_shader = compileShaderFromSourceFile(vertex_shader_source_file, GL_VERTEX_SHADER);
        fragment_shader = compileShaderFromSourceFile(fragment_shader_source_file, GL_FRAGMENT_SHADER);
    } catch (const ShaderCompilationException& e) {
        std::cout << e.what() << std::endl;
        if(e.hasShader()) {
            std::cout << getShaderCompileLog(e.shader()) << std::endl;
        }
        glfwTerminate();
        exit(1);
    }

    // create shader program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    // bind fragment shader outputs to buffers
    glBindFragDataLocation(shader_program, 0, "out_color");

    // link and use
    glLinkProgram(shader_program);
    glUseProgram(shader_program);

    // create vertex array
    // stores mapping from vertex buffer to shader positions
    GLuint vertex_array;
    glCreateVertexArrays(1, &vertex_array);

    glBindVertexArray(vertex_array);

    // create vertex buffer
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    std::cout << vertexBuffer << std::endl;

    // triangle coordinates
    float vertices[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.f, 0.f, // Top-left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.f, 0.f, // Top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.f, 1.f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.f, 1.f  // Bottom-left
    };

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    // move triangle coordinates into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // set vertexBuffer as active Buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy coordinates into active buffer

    // link vertex data to shader input
    GLint position_shader_attribute = glGetAttribLocation(shader_program, "position");
    glVertexAttribPointer(position_shader_attribute, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0); // also stores the current vertex buffer

    GLint color_shader_attribute = glGetAttribLocation(shader_program, "color");
    glVertexAttribPointer(color_shader_attribute, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));

    GLint texture_shader_attribute = glGetAttribLocation(shader_program, "texture");
    glVertexAttribPointer(texture_shader_attribute, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

    GLint time_uniform = glGetUniformLocation(shader_program, "time");
    GLint time_uniform_vertex = glGetUniformLocation(shader_program, "time_v");

    glEnableVertexAttribArray(position_shader_attribute);
    glEnableVertexAttribArray(color_shader_attribute);
    glEnableVertexAttribArray(texture_shader_attribute);

    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // initialize Textures
    GLuint elite_picture = loadAndBindTextureFromImageFile("resources/images/elite_dangerous.jpg", GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader_program, "eliteTex"), 0);

    GLuint kitten_picture = loadAndBindTextureFromImageFile("resources/images/kitten.png", GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader_program, "kittenTex"), 1);
    // end Textures

    GLint model_matrix_uniform = glGetUniformLocation(shader_program, "model");
    GLint view_matrix_uniform = glGetUniformLocation(shader_program, "view");
    GLint projection_matrix_uniform = glGetUniformLocation(shader_program, "projection");
    
    glm::mat4 view = glm::lookAt(
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
    glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, glm::value_ptr(proj));

    // begin of main loop logic
    auto t_start = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

        auto model = glm::mat4(1.f);
        model = glm::rotate(model,time * glm::radians(180.f), glm::vec3(0.f,0.f,1.f));
        glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1f(time_uniform, (sin(time* 4.f) + 1.f) / 2.f);
        glUniform1f(time_uniform_vertex, (sin(time* 4.f) + 1.f) / 2.f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }


    glfwTerminate();
}
