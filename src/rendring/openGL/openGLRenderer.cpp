#include "openGLRenderer.hpp"

openGLRenderer::openGLRenderer(openglContext *shared)
{
    renderThread = new std::thread(&openGLRenderer::renderHandle, this, shared);
}

openGLRenderer::~openGLRenderer()
{
    
}

void openGLRenderer::renderRequest(const renderRequestInfo& dataToRender)
{
    requests.push(dataToRender);
}



void openGLRenderer::renderHandle(openglContext *shared)
{
    openGLRenderer::context = new openglContext(shared->eglContext);
    context->makeCurrent();

    shapes = new vertexArrayManger(context);
    shaders = new shaderManger(context);
    textures = new textureManger(context);
    frameBuffers = new frameBuffersManger(context, textures);

    GL_CALL(context, Enable(GL_BLEND));
    GL_CALL(context, Enable(GL_DEPTH_TEST));
    GL_CALL(context, Enable(GL_ALPHA_TEST));
    GL_CALL(context, BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(context, AlphaFunc(GL_GREATER, 0));

    constexpr float vertices[] = {
        1.0f,    1.0f,    0.0f,
        1.0f,   -1.0f,    0.0f,
        -1.0f,  -1.0f,   0.0f, 
        -1.0f,   1.0f,   0.0f, 
    };  

    constexpr float vertices2[] = {
        1.0f, 1.0f,
        1.0f, 0,
        -0, 0,
        0.0f, 1.0f,
    };  
    
    constexpr uint32_t indcies[] = {
        0, 1, 2,
        0, 3, 2
    };

    vaoId test =  shapes->createVao();
    shapes->attachIndexBuffer(test, indcies, 6);

    shapes->addVertexBufferBinding(test, {
        0,
        vertices,
        sizeof(vertices),
        3 * sizeof(GLfloat)
    });

    shapes->addVertexBufferBinding(test, {
        1,
        vertices2,
        sizeof(vertices2),
        2 * sizeof(GLfloat)
    
    });

    shapes->addVertexBufferAttacment(test, 0, 0, 3);
    shapes->addVertexBufferAttacment(test, 1, 1, 2);

    while (context)
    {
        frameBuffers->handleRequsets();
        textures->handleRequsets();
        
        while(!requests.empty())
        {
            renderRequestInfo dataToRender = requests.front();
            frameBuffers->bind(dataToRender.frameBufferId);

            auto fboStatus = context->openGLAPI->CheckFramebufferStatus(GL_FRAMEBUFFER);
            CONDTION_LOG_ERROR("Framebuffer is incomplete!: " << std::hex << fboStatus,  fboStatus != GL_FRAMEBUFFER_COMPLETE)
            
            GL_CALL(context, Viewport(0, 0, frameBuffers->getWidth(dataToRender.frameBufferId), frameBuffers->getHight(dataToRender.frameBufferId)));
            GL_CALL(context, ClearColor (0.0f, 0.0f, 0.0f, 0.0f));
            GL_CALL(context, Clear (GL_COLOR_BUFFER_BIT));
            
    
            for (auto& drawCallData: dataToRender.drawCalls)
            {
                shapes->bind(test);
                // shaders->bind(drawCallData.shaderId);
                for (size_t i = 0; i < 32; i++)
                {
                    if(drawCallData.texturesIds[i].gen != 255)
                        textures->bind(drawCallData.texturesIds[i], i);
                }

                

                    // GL_CALL(context, NamedBufferData(VBO[0], sizeof(vertices), vertices, GL_DYNAMIC_DRAW));
                    // GL_CALL(context, VertexArrayVertexBuffer(VAO, 1, VBO[0], 0, 3 * sizeof(GLfloat)));

                    // GL_CALL(context, NamedBufferData(VBO[1], sizeof(vertices2), vertices2, GL_DYNAMIC_DRAW));
                    // GL_CALL(context, VertexArrayVertexBuffer(VAO, 0, VBO[1], 0, 2 * sizeof(GLfloat)));

                    // GL_CALL(context, EnableVertexArrayAttrib(VAO, 0));
                    // GL_CALL(context, VertexArrayAttribBinding(VAO, 0, 1));
                    // GL_CALL(context, VertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0));

                    // GL_CALL(context, EnableVertexArrayAttrib(VAO, 1));
                    // GL_CALL(context, VertexArrayAttribBinding(VAO, 1, 0));
                    // GL_CALL(context, VertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float)));


                // GL_CALL(context, DrawElements(GL_TRIANGLES, shapes->getCount(drawCallData.vertexArrayId), GL_UNSIGNED_INT, nullptr));
                GL_CALL(context, DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
                
                textures->rebuild(frameBuffers->getColorAttachmens(dataToRender.frameBufferId)[0]);
            }
            
            requests.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   
    }    
}
