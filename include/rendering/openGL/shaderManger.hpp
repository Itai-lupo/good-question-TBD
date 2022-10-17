#pragma once
#include "openglContext.hpp"


class shaderManger
{
    private:
        openglContext *context;
    
    public:
        shaderManger(openglContext *context): context(context){}
        ~shaderManger();
};
