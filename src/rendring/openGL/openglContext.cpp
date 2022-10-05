#include "openglContext.hpp"

#include <iostream>
#include <string>

#include "log.hpp"


#define GL_ERROR_TRANSLATE(e) case e: return #e;


openglContext::openglContext(EGLContext sharedContext)
{
    eglSwapInterval(eglDisplay, 0);
    eglBindAPI (EGL_OPENGL_API);

    EGLint attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    EGLint num_config;
    
    eglChooseConfig (eglDisplay, attributes, &eglConfig, 1, &num_config);
    
    eglContext = eglCreateContext (eglDisplay, eglConfig, sharedContext, NULL);
    CONDTION_LOG_FATAL("Failed to initialize eglContext!: " << std::hex << eglGetError() , eglContext == EGL_NO_CONTEXT);

    eglMakeCurrent (eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext);
    
    
    openGLAPI = (GladGLContext*) malloc(sizeof(GladGLContext));
    int version = gladLoaderLoadGLContext(openGLAPI);
    
    LOG_INFO("Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) );
    
}

void openglContext::GLClearErrors()
{
    while(openGLAPI->GetError() != GL_NO_ERROR);
}

std::string openglContext::GL_TranslateError (GLenum error)
{
    switch (error) {
        GL_ERROR_TRANSLATE(GL_INVALID_ENUM)
        GL_ERROR_TRANSLATE(GL_INVALID_VALUE)
        GL_ERROR_TRANSLATE(GL_INVALID_OPERATION)
        GL_ERROR_TRANSLATE(GL_OUT_OF_MEMORY)
        GL_ERROR_TRANSLATE(GL_NO_ERROR)
        GL_ERROR_TRANSLATE(GL_STACK_OVERFLOW)
        GL_ERROR_TRANSLATE(GL_STACK_UNDERFLOW) 
        GL_ERROR_TRANSLATE(GL_INVALID_FRAMEBUFFER_OPERATION)
        default:
            return "UNKNOWN";
    }
}

void openglContext::GLCheckError(const char *function, const char *file, int line)
{
    GLenum error;
    while((error = openGLAPI->GetError()) != GL_NO_ERROR)
    {
        LOG_INFO(
            "[OpenGL error] (" << GL_TranslateError(error) << ": " << error << "): " << function << " " << file <<  ":" << line);
    }
    
}
