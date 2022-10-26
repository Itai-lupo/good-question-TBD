#include "shaderManger.hpp"

#include <fstream>

shaderId shaderManger::createProgram(const std::string& vertex, const std::string& fragment)
{
    shaderId id;
    if(!freeSlots.empty())
    {
        id = {
            .gen = idToIndex[freeSlots.front()].gen,
            .index = freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .gen = 0,
            .index = (uint32_t)idToIndex.size()
        };
        idToIndex.push_back({0, (uint32_t)shaders.size()});
    }

    shaderInfo info;
    info.id = id;
    info.vertexPath = vertex;
    info.fragmentPath = fragment;

    idToIndex[id.index].index = shaders.size();
    shaders.push_back(info);
    return id;
}

void shaderManger::compileVertexShader(shaderInfo& shader)
{

    std::ifstream input (shader.vertexPath, std::fstream::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});


    shader.vertexShader = context->openGLAPI->CreateShader(GL_VERTEX_SHADER);
    context->openGLAPI->ShaderBinary(1, &shader.vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), buffer.size());
    context->openGLAPI->SpecializeShader(shader.vertexShader, "main", 0, nullptr, nullptr);

    GLint isCompiled = 0;
    context->openGLAPI->GetShaderiv(shader.vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        context->openGLAPI->GetShaderiv(shader.vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        context->openGLAPI->GetShaderInfoLog(shader.vertexShader, maxLength, &maxLength, infoLog.data());
        
        context->openGLAPI->DeleteShader(shader.vertexShader);

        LOG_INFO("did not compile: " << shader.vertexPath)
        LOG_FATAL("failed to compile shader: " << buffer.size() << ", " << (infoLog.data()))
    }

}

void shaderManger::compileFrgmentShader(shaderInfo& shader)
{
    std::ifstream input (shader.fragmentPath, std::fstream::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});


    shader.fragmentShader = context->openGLAPI->CreateShader(GL_FRAGMENT_SHADER);
    context->openGLAPI->ShaderBinary(1, &shader.fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), buffer.size());
    context->openGLAPI->SpecializeShader(shader.fragmentShader, "main", 0, nullptr, nullptr);

    GLint isCompiled = 0;
    context->openGLAPI->GetShaderiv(shader.fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        context->openGLAPI->GetShaderiv(shader.fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        context->openGLAPI->GetShaderInfoLog(shader.fragmentShader, maxLength, &maxLength, &infoLog[0]);
        
        context->openGLAPI->DeleteShader(shader.fragmentShader);

        LOG_FATAL("failed to compile shader: " << std::string(infoLog.data()))
    }
}

void shaderManger::linkProgram(shaderInfo& shader)
{
    shader.programId = context->openGLAPI->CreateProgram();

    context->openGLAPI->AttachShader(shader.programId, shader.vertexShader);
    context->openGLAPI->AttachShader(shader.programId, shader.fragmentShader);

    context->openGLAPI->LinkProgram(shader.programId);

    GLint isLinked = 0;
    context->openGLAPI->GetProgramiv(shader.programId, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        context->openGLAPI->GetProgramiv(shader.programId, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        context->openGLAPI->GetProgramInfoLog(shader.programId, maxLength, &maxLength, &infoLog[0]);
        
        context->openGLAPI->DeleteProgram(shader.programId);
        context->openGLAPI->DeleteShader(shader.vertexShader);
        context->openGLAPI->DeleteShader(shader.fragmentShader);

        return;
    }

    context->openGLAPI->DetachShader(shader.programId, shader.vertexShader);
    context->openGLAPI->DetachShader(shader.programId, shader.fragmentShader);
}

void shaderManger::bind(shaderId id)
{

    if(idToIndex[id.index].gen != id.gen)
        return;

    shaderInfo& temp = shaders[idToIndex[id.index].index];
    if(temp.programId == 0)
    {
        compileVertexShader(temp);
        compileFrgmentShader(temp);
        linkProgram(temp);
    }
    context->openGLAPI->UseProgram(temp.programId);
}