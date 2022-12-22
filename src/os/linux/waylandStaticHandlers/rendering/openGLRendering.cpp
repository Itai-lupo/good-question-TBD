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
            LOG_FATAL("ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog);
        }
    }
    else
    {
        GL_CALL(openGLRendering::context, GetProgramiv(shader, GL_LINK_STATUS, &success));
        if (!success)
        {
            GL_CALL(openGLRendering::context, GetProgramInfoLog(shader, 1024, NULL, infoLog));
            LOG_FATAL("ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog);
        }
    }
}


void initTextureShader()
{
    std::string vertexCode;
    std::string fragmentCode;
    
    constexpr char* fShaderCode = 
        (char*)"#version 460 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
            "FragColor = texture(ourTexture, TexCoord);\n"
        "}\n";

    constexpr char * vShaderCode = 
        (char*)"#version 460 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "layout (location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(pos, 1.0);\n"
        "    TexCoord = texCoord;\n"
        "}\n";

    unsigned int vertex, fragment;

    unsigned int shaderID;

    vertex = openGLRendering::context->openGLAPI->CreateShader(GL_VERTEX_SHADER);
    GL_CALL(openGLRendering::context, ShaderSource(vertex, 1, &vShaderCode, NULL));
    GL_CALL(openGLRendering::context, CompileShader(vertex));
    checkCompileErrors(vertex, "VERTEX");

    fragment = openGLRendering::context->openGLAPI->CreateShader(GL_FRAGMENT_SHADER);
    GL_CALL(openGLRendering::context, ShaderSource(fragment, 1, &fShaderCode, NULL));
    GL_CALL(openGLRendering::context, CompileShader(fragment));
    checkCompileErrors(fragment, "FRAGMENT");
    
    shaderID = openGLRendering::context->openGLAPI->CreateProgram();
    GL_CALL(openGLRendering::context, AttachShader(shaderID, vertex));
    GL_CALL(openGLRendering::context, AttachShader(shaderID, fragment));
    GL_CALL(openGLRendering::context, LinkProgram(shaderID));
    checkCompileErrors(shaderID, "PROGRAM");
    
    GL_CALL(openGLRendering::context, DeleteShader(vertex));
    GL_CALL(openGLRendering::context, DeleteShader(fragment));
    GL_CALL(openGLRendering::context, UseProgram(shaderID)); 

    GL_CALL(openGLRendering::context, Uniform1i(openGLRendering::context->openGLAPI->GetUniformLocation(shaderID, "texture"), 0)); 
}


void openGLRendering::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{

    ZoneScoped;
    wl_callback_destroy(cb);
    surfaceId id = *(surfaceId*)data;
    renderInfo *temp = renderData->getComponent(id);
    if(!temp)
        return;
    
    

    
    std::unique_lock lk2{*temp->renderMutex.get()};
    temp->renderFinshed->wait(lk2, [&](){ return temp->renderFinshedBool;} );
    temp->renderFinshedBool = false;    
    
    cb = wl_surface_frame(surface::getSurface(id));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);
    
    context->makeCurrent(temp->eglSurface, temp->eglSurface);

    GL_CALL(context, Viewport(0, 0, surface::getWindowWidth(id), surface::getWindowHeight(id)));
    GL_CALL(context, ClearColor (0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(context, Clear (GL_COLOR_BUFFER_BIT));
    
    GL_CALL(context, BindTextureUnit(0, openGLRenderEngine::textures::getTexture(temp->bufferToRenderTex)->renderId));
    
    GL_CALL(context, DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    GL_CALL(context, BindTexture(GL_TEXTURE_2D, 0));

    context->swapBuffers(temp->eglSurface);
    wl_surface_commit(surface::getSurface(id));
        
    framebufferId tempBufferIndex = temp->bufferInRender;
    temp->bufferInRender = temp->bufferToRender;
    temp->bufferToRender = tempBufferIndex;

    textureId tempTexIndex = temp->bufferInRenderTex;
    temp->bufferInRenderTex = temp->bufferToRenderTex;
    temp->bufferToRenderTex = tempTexIndex;    
    
#ifdef TRACY_ENABLE
    // TracyMessage((std::string("wait ticks count: ") + std::to_string(i)).c_str(), (std::string("wait ticks count: ") + std::to_string(i)).size());
    ZoneText(toplevel::getWindowTitle(id).c_str(), toplevel::getWindowTitle(id).size());
    if(&renderData->getData().back() == temp){
        FrameMarkNamed("openGLRendering");
    }
#endif
}

void openGLRendering::init(entityPool *surfacesPool, renderApi *api)
{
    openglContext::setDisplay(eglGetDisplay(linuxWindowAPI::display));

    context = new openglContext();
    openGLRenderEngine::openGLRenderer::init(context);

    renderData = new gpuRenderInfoComponent(surfacesPool);
    openGLRendering::api = api;

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
    
    GL_CALL(context, CreateVertexArrays(1, &VAO));
    GL_CALL(context, CreateBuffers(1, &VBO));
    
    GL_CALL(context, NamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW));


    GL_CALL(context, EnableVertexArrayAttrib(VAO, 0));
    GL_CALL(context, VertexArrayAttribBinding(VAO, 0, 0));
    GL_CALL(context, VertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0));

    GL_CALL(context, EnableVertexArrayAttrib(VAO, 1));
    GL_CALL(context, VertexArrayAttribBinding(VAO, 1, 0));
    GL_CALL(context, VertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float)));
    GL_CALL(context, VertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat)));

    constexpr uint32_t indcies[] = {
        0, 1, 2,
        0, 3, 2
    };

    uint32_t IBO;
    GL_CALL(context, CreateBuffers(1, &IBO));
    GL_CALL(context, NamedBufferData(IBO, 6 * sizeof(unsigned int), indcies, GL_STATIC_DRAW));
    GL_CALL(context, VertexArrayElementBuffer(VAO, IBO));

    GL_CALL(context, BindVertexArray(VAO));

    initTextureShader();
}

void openGLRendering::close()
{
    openGLRenderEngine::openGLRenderer::close();
    delete context;
    delete renderData;
}

void openGLRendering::allocateSurfaceToRender(surfaceId winId, void(*callback)(const gpuRenderData&))
{ 
    renderInfo info;
    info.id = winId;
    info.renderFuncion = callback;
    renderData->setComponent(winId, info);
    renderData->getComponent(winId)->renderThread = new std::thread(renderWindow, winId);


    wl_callback *cb = wl_surface_frame(surface::getSurface(winId));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new surfaceId(winId));
}

void openGLRendering::setRenderEventListeners(surfaceId winId, void(*callback)(const gpuRenderData&))
{
    renderData->getComponent(winId)->renderFuncion = callback;
}

void openGLRendering::deallocateSurfaceToRender(surfaceId winId)
{

    renderData->deleteComponent(winId);
}

void openGLRendering::resize(surfaceId id, int width, int height)
{
    renderInfo *temp = renderData->getComponent(id);

    if(temp->eglWindow == NULL){
        
        temp->eglWindow = wl_egl_window_create (surface::getSurface(id), width, height);
        temp->eglSurface = eglCreateWindowSurface (context->eglDisplay, context->eglConfig, (EGLNativeWindowType)temp->eglWindow, NULL);
        context->makeCurrent(temp->eglSurface, temp->eglSurface);

        temp->bufferInRenderTex = api->allocTexture(supportedRenderApis::openGl);
        temp->bufferToRenderTex = api->allocTexture(supportedRenderApis::openGl);
        temp->freeBufferTex = api->allocTexture(supportedRenderApis::openGl);

        textureInfo tempTex(temp->bufferInRenderTex, width, height, textureFormat::RGBA8);
        openGLRenderEngine::textures::setTextureData(tempTex);
        
        tempTex.id = temp->bufferToRenderTex;
        openGLRenderEngine::textures::setTextureData(tempTex);

        tempTex.id = temp->freeBufferTex;
        openGLRenderEngine::textures::setTextureData(tempTex);
        

        temp->bufferInRender = api->allocFramebuffer(supportedRenderApis::openGl);
        temp->bufferToRender = api->allocFramebuffer(supportedRenderApis::openGl);
        temp->freeBuffer = api->allocFramebuffer(supportedRenderApis::openGl);

        frameBufferInfo tempFBO(temp->bufferInRender, width, height);

        tempFBO.colorAttachmens[0] = temp->bufferInRenderTex;
        openGLRenderEngine::framebuffers::setFrameBufferData(tempFBO);

        tempFBO.id = temp->bufferToRender; 
        tempFBO.colorAttachmens[0] = temp->bufferToRenderTex;
        openGLRenderEngine::framebuffers::setFrameBufferData(tempFBO);
        
        tempFBO.id = temp->freeBuffer; 
        tempFBO.colorAttachmens[0] = temp->freeBufferTex;
        openGLRenderEngine::framebuffers::setFrameBufferData(tempFBO);

        context->swapBuffers(temp->eglSurface);
    }

    wl_egl_window_resize (temp->eglWindow, width, height, 0, 0);

    for (auto id: {temp->bufferInRender, temp->bufferToRender, temp->freeBuffer})
    {
        frameBufferInfo *FBO = openGLRenderEngine::framebuffers::getFrameBuffer(id);
        FBO->width = width;
        FBO->height = height;
        FBO->needToRebuild = true;
    }
    
}

void openGLRendering::renderWindow(surfaceId id)
{

    ZoneScoped;
    
    std::string thradNameA = "render " + toplevel::getWindowTitle(id);
    tracy::SetThreadName(thradNameA.c_str());
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();


    renderInfo *temp = renderData->getComponent(id);
    while (temp->freeBuffer.gen == 255)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    do {
        ZoneScoped;

        std::chrono::duration<double, std::milli> time_span = t2 - t1;
        t1 = std::chrono::high_resolution_clock::now();
        

        int elpased = time_span.count();
        framebufferId bufferIndex = temp->freeBuffer;
        
        temp->renderFuncion(gpuRenderData{id, surface::getWindowWidth(id), surface::getWindowHeight(id), elpased, api, bufferIndex, supportedRenderApis::openGl});
        
        framebufferId tempBufferIndex = temp->freeBuffer;
        temp->freeBuffer = temp->bufferToRender;
        temp->bufferToRender = tempBufferIndex;

        textureId tempTexIndex = temp->freeBufferTex;
        temp->freeBufferTex = temp->bufferToRenderTex;
        temp->bufferToRenderTex = tempTexIndex;
        
        
        temp->renderFinshedBool = true;
        std::shared_lock lk{*temp->renderMutex.get()};
        temp->renderFinshed->notify_one();
        
        t2 = std::chrono::high_resolution_clock::now();
    
        temp = renderData->getComponent(id);
    } while (temp);    
}


#endif