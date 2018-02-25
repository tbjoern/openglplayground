#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

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
    std::string readFile(const std::string &fileName)
    {
        using namespace std;
        ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);
        ifs.exceptions ( ifstream::failbit | ifstream::badbit );

        ifstream::pos_type fileSize = ifs.tellg();
        ifs.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return string(bytes.data(), fileSize);
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
        0.0f,  0.5f, 1.f, 0.f, 0.f, // Vertex 1 (X, Y) r,g,b
        0.5f, -0.5f, 0.f, 1.f, 0.f, // Vertex 2 (X, Y) r,g,b
        -0.5f, -0.5f, 0.f, 0.f, 1.f  // Vertex 3 (X, Y) r,g,b
    };

    // move triangle coordinates into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // set vertexBuffer as active Buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy coordinates into active buffer

    // link vertex data to shader input
    GLint position_shader_attribute = glGetAttribLocation(shader_program, "position");
    glVertexAttribPointer(position_shader_attribute, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0); // also stores the current vertex buffer

    GLint color_shader_attribute = glGetAttribLocation(shader_program, "color");
    glVertexAttribPointer(color_shader_attribute, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));

    glEnableVertexAttribArray(position_shader_attribute);
    glEnableVertexAttribArray(color_shader_attribute);

    while(!glfwWindowShouldClose(window))
    {
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }


    glfwTerminate();
}
