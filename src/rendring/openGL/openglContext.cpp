#include "openglContext.hpp"

#include <iostream>
#include <string>

#include "log.hpp"


#define GL_ERROR_TRANSLATE(e) case e: return #e;

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam );

openglContext::openglContext(EGLContext sharedContext)
{
    eglSwapInterval(eglDisplay, 0);
    eglBindAPI (EGL_OPENGL_API);

    EGLint attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
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
    
    openGLAPI->Enable( GL_DEBUG_OUTPUT );
    openGLAPI->Enable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    openGLAPI->DebugMessageCallback( MessageCallback, 0 );
    openGLAPI->DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

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
    return;
    while((error = openGLAPI->GetError()) != GL_NO_ERROR)
    {
        LOG_INFO(
            "[OpenGL error] (" << GL_TranslateError(error) << ": " << error << "): " << function << " " << file <<  ":" << line);
    }
    
}

std::string GL_TranslateError (GLenum error)
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

std::string GL_TranslateSeverity (GLenum error)
{
    switch (error) {
        GL_ERROR_TRANSLATE(GL_DEBUG_SEVERITY_HIGH)
        GL_ERROR_TRANSLATE(GL_DEBUG_SEVERITY_LOW)
        GL_ERROR_TRANSLATE(GL_DEBUG_SEVERITY_MEDIUM)
        GL_ERROR_TRANSLATE(GL_DEBUG_SEVERITY_NOTIFICATION)
        default:
            return "UNKNOWN";
    }
}

std::string GL_TranslateType (GLenum error)
{
    switch (error) {
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_ERROR)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_MARKER)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_OTHER)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_PERFORMANCE)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_POP_GROUP)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_PORTABILITY)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_PUSH_GROUP)
            GL_ERROR_TRANSLATE(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        default:
            return "UNKNOWN";
    }
}

std::string GL_TranslateSource (GLenum error)
{
    switch (error) {
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_API)
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_SHADER_COMPILER)
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_THIRD_PARTY)
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_APPLICATION)
            GL_ERROR_TRANSLATE(GL_DEBUG_SOURCE_OTHER)
        default:
            return "UNKNOWN";
    }
}




void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    std::string debugTypeMsg = (type == GL_DEBUG_TYPE_ERROR) ? "GL ERROR" : "GL CALLBACK"; 
    CONDTION_LOG_ERROR( debugTypeMsg << ": "
        "\n\t\tSource: " << GL_TranslateSource(source) << 
        "\n\t\tType: " << GL_TranslateType(type) << " = 0x" << std::hex << type <<
        "\n\t\tSeverity: " << GL_TranslateSeverity(severity) <<  " = 0x" << std::hex << severity << 
        "\n\t\tMessage = " << message, severity != GL_DEBUG_SEVERITY_NOTIFICATION);
}
