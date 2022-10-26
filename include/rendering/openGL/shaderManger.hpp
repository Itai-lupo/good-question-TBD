#pragma once
#include "openglContext.hpp"
#include "core.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <string>

class shaderManger
{
    private:
        openglContext *context;
        
        struct shaderInfo
        {
            uint32_t programId = 0;
            uint32_t vertexShader, fragmentShader;

            shaderId id;

            std::string vertexPath, fragmentPath;
            bool needToRebuild;
        };


        std::vector<shaderInfo> shaders;

        struct idIndexes
        {
            uint32_t gen: 8;
            uint32_t index: 24;
        };        

        std::vector<idIndexes> idToIndex;  
        std::list<uint32_t> freeSlots;

        void compileVertexShader(shaderInfo& shader);
        void compileFrgmentShader(shaderInfo& shader);
        void linkProgram(shaderInfo& shader);

    public:
        shaderManger(openglContext *context): context(context){}
        ~shaderManger();

        shaderId createProgram(const std::string& vertexPath, const std::string& fragmentPath);

        void bind(shaderId id);
};
