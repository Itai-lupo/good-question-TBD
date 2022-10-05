#pragma once
#include <glad/gl.h>
#include <EGL/egl.h>

#include "log.hpp"


class openglContext
{
    private:
        
        std::string GL_TranslateError (GLenum error);
    public:
        static inline EGLDisplay eglDisplay;
        EGLConfig eglConfig;
        EGLContext eglContext;
        GladGLContext *openGLAPI;

        void GLClearErrors();

        void GLCheckError(const char *function, const char *file, int line);

        static void setDisplay(EGLDisplay eglDisplay)
        {
            openglContext::eglDisplay = eglDisplay;
            eglInitialize (eglDisplay, NULL, NULL);
        }

        openglContext( EGLContext sharedContext = EGL_NO_CONTEXT);
        ~openglContext();

        void makeCurrent(EGLSurface draw = EGL_NO_SURFACE, EGLSurface read = EGL_NO_SURFACE)
        {
            eglMakeCurrent (eglDisplay, draw, read, eglContext);
        }

        void swapBuffers(EGLSurface s)
        {
            eglSwapBuffers(eglDisplay, s);
        }
};

#define GL_CALL(context, call) \
    context->GLClearErrors(); \
    context->openGLAPI->call; \
    context->GLCheckError(#call, __FILE__, __LINE__);
