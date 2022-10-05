#ifdef __linux__

#include "openGLRendering.hpp"
#include "surface.hpp"
#include "log.hpp"
#include "toplevel.hpp"
#include "layer.hpp"
#include "linuxWindowAPI.hpp"

#include <sys/mman.h>
#include <sstream>
#include <sys/prctl.h>
#include <errno.h>
#include <fcntl.h>
#include <utility>
#include <Tracy.hpp>


#include <glad/gl.h>
#include <EGL/egl.h>


#include <fstream>
#include <sstream>
#include <iostream>
class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = openGLRendering::context->openGLAPI->CreateShader(GL_VERTEX_SHADER);
        GL_CALL(openGLRendering::context, ShaderSource(vertex, 1, &vShaderCode, NULL));
        GL_CALL(openGLRendering::context, CompileShader(vertex));
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = openGLRendering::context->openGLAPI->CreateShader(GL_FRAGMENT_SHADER);
        GL_CALL(openGLRendering::context, ShaderSource(fragment, 1, &fShaderCode, NULL));
        GL_CALL(openGLRendering::context, CompileShader(fragment));
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = openGLRendering::context->openGLAPI->CreateProgram();
        GL_CALL(openGLRendering::context, AttachShader(ID, vertex));
        GL_CALL(openGLRendering::context, AttachShader(ID, fragment));
        GL_CALL(openGLRendering::context, LinkProgram(ID));
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        GL_CALL(openGLRendering::context, DeleteShader(vertex));
        GL_CALL(openGLRendering::context, DeleteShader(fragment));
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        GL_CALL(openGLRendering::context, UseProgram(ID)); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        GL_CALL(openGLRendering::context, Uniform1i(openGLRendering::context->openGLAPI->GetUniformLocation(ID, name.c_str()), (int)value)); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        GL_CALL(openGLRendering::context, Uniform1i(openGLRendering::context->openGLAPI->GetUniformLocation(ID, name.c_str()), value)); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        GL_CALL(openGLRendering::context, Uniform1f(openGLRendering::context->openGLAPI->GetUniformLocation(ID, name.c_str()), value)); 
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            GL_CALL(openGLRendering::context, GetShaderiv(shader, GL_COMPILE_STATUS, &success));
            if (!success)
            {
                GL_CALL(openGLRendering::context, GetShaderInfoLog(shader, 1024, NULL, infoLog));
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            GL_CALL(openGLRendering::context, GetProgramiv(shader, GL_LINK_STATUS, &success));
            if (!success)
            {
                GL_CALL(openGLRendering::context, GetProgramInfoLog(shader, 1024, NULL, infoLog));
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

static float red = 0.0;

void shit(uint8_t index)
{
    std::string thradNameA = "open gl";
    prctl(PR_SET_NAME, thradNameA.c_str());
    ZoneScoped;
    openGLRendering::renderInfo& temp = openGLRendering::surfacesToRender[index];
    

    while (!temp.textureBufferId)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    openglContext *context = new openglContext(openGLRendering::context->eglContext);
    GL_CALL(context, Enable(GL_BLEND));
    GL_CALL(context, Enable(GL_DEPTH_TEST));
    GL_CALL(context, Enable(GL_ALPHA_TEST));
    GL_CALL(context, BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(context, AlphaFunc(GL_GREATER, 0));

    
    int red = 0;
    while (true)
    {
        red++;
        FrameMarkNamed( "red");
        ZoneScoped;

        
        

        int width = surface::getWindowWidth(temp.id), height = surface::getWindowHeight(temp.id);
        uint8_t *data = new uint8_t[ width * height * 4];
        for (size_t i = 0; i < width * height * 4; i += 4)
        {
            data[i + 0] = (i + red);
            data[i + 1] = 0;
            data[i + 2] = 0;
            data[i + 3] = 0xff;
        }
        {
        ZoneScopedN("open gl shit");
        
        GL_CALL(context, BindTexture(GL_TEXTURE_2D, temp.textureBufferId));  

        GL_CALL(context, TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height , 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GL_CALL(context, GenerateMipmap(GL_TEXTURE_2D));
        }

        delete[] data;
    }    
}

void openGLRendering::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{

    ZoneScoped;
    wl_callback_destroy(cb);
    surfaceId id = *(surfaceId*)data;
    
    
    uint32_t index = idToIndex[id.index].renderDataIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    renderInfo& temp = surfacesToRender[index];
    

    
    if(idToIndex[id.index].renderEventIndex != (uint8_t)-1)
    {
        // std::unique_lock lk2{*temp.renderMutex.get()};
        // temp.renderFinshed->wait(lk2, [&](){ return temp.renderFinshedBool;} );
        // temp.renderFinshedBool = false;
    }
    
    
    red = (red + 0.005f) < 1.0f? (red + 0.001f): 0 ;

    cb = wl_surface_frame(surface::getSurface(id));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);
    
    context->makeCurrent(temp.eglSurface, temp.eglSurface);

    GL_CALL(context, Viewport(0, 0, surface::getWindowWidth(id), surface::getWindowHeight(id)));
    GL_CALL(context, ClearColor (0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(context, Clear (GL_COLOR_BUFFER_BIT));
    
    GL_CALL(context, ActiveTexture(GL_TEXTURE0));
    GL_CALL(context, BindTexture(GL_TEXTURE_2D, temp.textureBufferId));

    GL_CALL(context, DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    GL_CALL(context, BindTexture(GL_TEXTURE_2D, 0));

    context->swapBuffers(temp.eglSurface);
    wl_surface_commit(surface::getSurface(id));
    

    FrameMarkNamed( toplevel::getWindowTitle(id).c_str());

}

void openGLRendering::init()
{
    openglContext::setDisplay(eglGetDisplay(linuxWindowAPI::display));

    context = new openglContext();
    
    GL_CALL(context, Enable(GL_BLEND));
    GL_CALL(context, Enable(GL_DEPTH_TEST));
    GL_CALL(context, Enable(GL_ALPHA_TEST));
    GL_CALL(context, BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(context, AlphaFunc(GL_GREATER, 0));


    constexpr float vertices[] = {
        1.0f,    1.0f,    0.0f,          1.0f, 1.0f,
        1.0f,   -1.0f,    0.0f,          1.0f, 0,
        -1.0f,  -1.0f,   0.0f,           0, 0,
        -1.0f,   1.0f,   0.0f,           0.0f, 1.0f,
    };  
    unsigned int VBO, VAO;
    
    GL_CALL(context, GenVertexArrays(1, &VAO));
    GL_CALL(context, GenBuffers(1, &VBO));
    GL_CALL(context, BindVertexArray(VAO));

    GL_CALL(context, BindBuffer(GL_ARRAY_BUFFER, VBO));  
    GL_CALL(context, BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));


    GL_CALL(context, VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
    GL_CALL(context, EnableVertexAttribArray(0));

    GL_CALL(context, VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
    GL_CALL(context, EnableVertexAttribArray(1));

    constexpr uint32_t indcies[] = {
        0, 1, 2,
        0, 3, 2
    };

    uint32_t IBO;
    GL_CALL(context, GenBuffers(1, &IBO));
    GL_CALL(context, BindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
    GL_CALL(context, BufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indcies, GL_STATIC_DRAW));


    textureShader = new Shader("assets/shaders/texture.vert", "assets/shaders/texture.frag");
    textureShader->use();
    textureShader->setInt("texture", 0);
}


void openGLRendering::allocateSurfaceToRender(surfaceId winId)
{ 
    if(winId.index >= idToIndex.size())
        idToIndex.resize(winId.index + 1);

    idToIndex[winId.index].gen = winId.gen;

    renderInfo info;
    info.id = winId;


    idToIndex[winId.index].renderDataIndex = surfacesToRender.size();
    surfacesToRender.push_back(info);

    std::thread(shit, idToIndex[winId.index].renderDataIndex).detach();


    wl_callback *cb = wl_surface_frame(surface::getSurface(winId));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new surfaceId(winId));
}


void openGLRendering::setRenderEventListeners(surfaceId winId, std::function<void(const windowRenderData&)> callback){
    uint32_t index = idToIndex[winId.index].renderEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        renderEventListeners[index] = callback;
        renderEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].renderEventIndex = renderEventListeners.size();
    renderEventListeners.push_back(callback);
    renderEventId.push_back(winId);
}     



void openGLRendering::deallocateSurfaceToRender(surfaceId winId)
{
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    unsetRenderEventListeners(winId);

    idToIndex[winId.index].renderDataIndex = -1;
    idToIndex[winId.index].gen = -1;

}


void openGLRendering::unsetRenderEventListeners(surfaceId winId)
{

    uint32_t index = idToIndex[winId.index].renderEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = renderEventListeners.size() - 1;
    idToIndex[renderEventId[lastIndex].index].renderEventIndex = index;
    renderEventListeners[index] = renderEventListeners[lastIndex];
    renderEventId[index] = renderEventId[lastIndex];

    renderEventListeners.pop_back();
    renderEventId.pop_back();

    idToIndex[winId.index].renderEventIndex = -1;
}

#endif