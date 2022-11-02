#include "openGLRenderer.hpp"

#include <sys/prctl.h>
#include <Tracy.hpp>
#include <stb_image.h>

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
    ZoneScoped;

    std::string thradNameA = "opengl";
    tracy::SetThreadName("opengl");
    prctl(PR_SET_NAME, thradNameA.c_str());
    
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
    // GL_CALL(context, Enable(GL_PROGRAM_POINT_SIZE));
    // GL_CALL(context, PointSize(10));

    constexpr float vertices3[] = {
        0.5f,    0.5f,    0.0f,          1.0f, 1.0f,
        0.5f,   -0.5f,    0.0f,          1.0f, 0,
        -0.5f,  -0.5f,   0.0f,           0, 0,
        -0.5f,   0.5f,   0.0f,           0.0f, 1.0f,
    };  

    constexpr float vertices1[] = {
        0.5f,    0.5f,    0.0f,           
        0.5f,   -0.5f,    0.0f,           
        -0.5f,  -0.5f,   0.0f,           
        -0.5f,   0.5f,   0.0f,            
    };  

    constexpr float vertices2[] = {
        1.0f, 1.0f,
        1.0f, 0,
        0, 0,
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
        vertices3,
        sizeof(vertices3),
        5 * sizeof(GLfloat)
    });


    shapes->addVertexBufferAttacment(test, 0, 0, 3);
    shapes->addVertexBufferAttacment(test, 1, 0, 2, 3 * sizeof(GLfloat));

    shaderId testShader = shaders->createProgram("assets/shaders/vertex/texture.vert.spv", "assets/shaders/fragment/texture.frag.spv");
    
    

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    
    int width, height, channels;
    data = stbi_load("assets/textures/preview-simple-dungeon-crawler-set1.png", &width, &height, &channels, 0);
    textureId testTexture = textures->createTexture(textureFormat::RGB8, width, height);
    textures->loadBuffer(testTexture, 0, 0, width, height, textureFormat::RGB8, GL_UNSIGNED_BYTE, data);
        

    while (context)
    {
        ZoneScopedN("render cycle");

        frameBuffers->handleRequsets();
        textures->handleRequsets();
        
        while(!requests.empty())
        {
            ZoneScopedN("render requst");

            renderRequestInfo dataToRender = requests.front();
            frameBuffers->bind(dataToRender.frameBufferId);

            auto fboStatus = context->openGLAPI->CheckFramebufferStatus(GL_FRAMEBUFFER);
            CONDTION_LOG_ERROR("Framebuffer is incomplete!: " << std::hex << fboStatus,  fboStatus != GL_FRAMEBUFFER_COMPLETE)
            
            GL_CALL(context, Viewport(0, 0, frameBuffers->getWidth(dataToRender.frameBufferId), frameBuffers->getheight(dataToRender.frameBufferId)));
            GL_CALL(context, ClearColor (0.2f, 0.2f, 0.2f, 1.0f));
            GL_CALL(context, Clear (GL_COLOR_BUFFER_BIT));
            
            ZoneValue(dataToRender.frameBufferId.gen << 24 + dataToRender.frameBufferId.index);
            for (auto& drawCallData: dataToRender.drawCalls)
            {
                ZoneScopedN("draw call");

                drawCallData.vertexArrayId = test;
                drawCallData.shader = testShader;
                shapes->bind(drawCallData.vertexArrayId);
                shaders->bind(drawCallData.shader);
            

                drawCallData.texturesIds[0] = testTexture;
                for (size_t i = 0; i < 32; i++)
                {
                    ZoneScopedN("texture bind");
                    if(drawCallData.texturesIds[i].gen != 255)
                        textures->bind(drawCallData.texturesIds[i], i);
                }

                
                GL_CALL(context, DrawElements(GL_TRIANGLES, shapes->getCount(drawCallData.vertexArrayId), GL_UNSIGNED_INT, nullptr));
                GL_CALL(context, DrawElements(GL_POINTS, shapes->getCount(drawCallData.vertexArrayId), GL_UNSIGNED_INT, nullptr));
                
                textures->rebuild(frameBuffers->getColorAttachmens(dataToRender.frameBufferId)[0]);
            }
            requests.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));   
    }    
}
