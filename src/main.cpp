#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <exception>

namespace {

    class ShaderCompilationException : public std::exception {
    private:
        std::string _shader_source_filename;
        std::string _additional_info;
        GLuint _shader;
        bool _shader_set;
    public:
        ShaderCompilationException(std::string shader_source_filename, std::string additional_info="") : _shader_source_filename(shader_source_filename), _additional_info(additional_info), _shader(), _shader_set(true) {};

        ShaderCompilationException(std::string shader_source_filename, GLuint shader, std::string additional_info="") : _shader_source_filename(shader_source_filename), _additional_info(additional_info), _shader(shader), _shader_set(true) {};

        virtual ~ShaderCompilationException() = default;

        const char* what() const noexcept override {
            return ("Shader compilation failed: " + _shader_source_filename).c_str();
        }

        bool hasShader() const {
            return _shader_set;
        }

        GLuint shader() const {
            return _shader;
        }
    };

    char* readFile(const std::string &fileName)
    {
        using namespace std;
        ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);
        ifs.exceptions ( ifstream::failbit | ifstream::badbit );

        ifstream::pos_type fileSize = ifs.tellg();
        ifs.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return bytes.data();
    }

    GLuint compileShaderFromSourceFile(const std::string& filename, GLenum shader_type) {
        char* shader_source;
        try {
           shader_source = readFile(filename);
        } catch (const std::ifstream::failure& e) {
            throw ShaderCompilationException(filename, "Could not read file");
        }
        GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shader_source, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_TRUE) {
            return shader;
        } else {
            throw ShaderCompilationException(filename, "Compilation Error");
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

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    std::cout << vertexBuffer << std::endl;

    // draw a triangle
    float vertices[] = {
        0.0f,  0.5f, // Vertex 1 (X, Y)
        0.5f, -0.5f, // Vertex 2 (X, Y)
        -0.5f, -0.5f  // Vertex 3 (X, Y)
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }


    glfwTerminate();
}
