#include "openGLRenderer.hpp"

#include <sys/prctl.h>
#include <Tracy.hpp>


#include <atomic>

namespace openGLRenderEngine
{
    std::atomic_bool canPush = true;

    void openGLRenderer::init(openglContext *shared)
    {
        renderThread = new std::thread(renderHandle, shared);
    }

    void openGLRenderer::close()
    {
        
    }

    void openGLRenderer::renderRequest(const renderRequestInfo& dataToRender)
    {
        while (!canPush)
            std::this_thread::sleep_for(std::chrono::microseconds(2));
        canPush = false;
        
        requests.push(dataToRender);
        canPush = true;

    }

    void openGLRenderer::renderHandle(openglContext *shared)
    {
        ZoneScoped;

        std::string thradNameA = "opengl";
        tracy::SetThreadName("opengl");
        prctl(PR_SET_NAME, thradNameA.c_str());
        
        openGLRenderer::context = new openglContext(shared->eglContext);
        context->makeCurrent();

        framebuffers::setContext(context);
        shaders::setContext(context);
        textures::setContext(context);
        vaos::setContext(context);
        
        GL_CALL(context, Enable(GL_BLEND));
        GL_CALL(context, Enable(GL_DEPTH_TEST));
        GL_CALL(context, Enable(GL_ALPHA_TEST));
        GL_CALL(context, BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GL_CALL(context, AlphaFunc(GL_GREATER, 0));
        GL_CALL(context, Enable(GL_PROGRAM_POINT_SIZE));
        GL_CALL(context, PointSize(10));



        while (context)
        {
            ZoneScopedN("render cycle");

            framebuffers::handleRequsets();
            shaders::handleRequsets();
            textures::handleRequsets();
            vaos::handleRequsets();
            
            while (!canPush)
                std::this_thread::sleep_for(std::chrono::microseconds(2));
            canPush = false;
            while(!requests.empty())
            {
                ZoneScopedN("render requst");

                renderRequestInfo dataToRender = requests.front();
                framebuffers::bind(dataToRender.frameBufferId);

                GL_CALL(context, Viewport(0, 0, 
                    framebuffers::getFrameBuffer(dataToRender.frameBufferId)->width, 
                    framebuffers::getFrameBuffer(dataToRender.frameBufferId)->height));

                GL_CALL(context, ClearColor (0.2f, 0.2f, 0.2f, 1.0f));
                GL_CALL(context, Clear (GL_COLOR_BUFFER_BIT));
                
                ZoneValue(dataToRender.frameBufferId.gen << (24 + dataToRender.frameBufferId.index));
                for (auto& drawCallData: dataToRender.drawCalls)
                {
                    ZoneScopedN("draw call");

                    vaos::bind(drawCallData.vertexArrayId);
                    shaders::bind(drawCallData.shader);
                

                    for (size_t i = 0; i < 32; i++)
                    {
                        ZoneScopedN("texture bind");
                        if(drawCallData.texturesIds[i].gen != 255)
                            textures::bind(drawCallData.texturesIds[i], i);
                    }

                    uint32_t count = vaos::getVAO(drawCallData.vertexArrayId)->count;
                    GL_CALL(context, DrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
                    GL_CALL(context, DrawElements(GL_POINTS, count, GL_UNSIGNED_INT, nullptr));
                }
                GL_CALL(context, Flush());
                requests.pop();
            }
            canPush = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));   
        }    
    }
}