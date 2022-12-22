#include "textureManger.hpp"
#include <Tracy.hpp>


namespace openGLRenderEngine
{

    void textures::init(entityPool *texturesPool)
    {
        texturesData = new textureComponents(texturesPool);
    }

    void textures::close()
    {
        delete texturesData;
    }
    
    void textures::setContext(openglContext *context)
    {
        textures::context = context;
    }

    void textures::handleRequsets()
    {
        if(toDelete.empty())
            return;

        uint32_t *texturesToDelete = new uint32_t[toDelete.size()];
        uint32_t temp = toDelete.size();

        for (size_t i = 0; i < temp; i++)
        {
            texturesToDelete[i] = toDelete.front();
            toDelete.pop();
        }
        
        GL_CALL(context, DeleteTextures(temp, texturesToDelete)); 
    }


    void textures::setTextureData(textureInfo info)
    {
        info.needToRebuild = true;
        texturesData->setComponent(info.id, info);
    }

    textureInfo *textures::getTexture(textureId id)
    {
        return texturesData->getComponent(id);
    }

    void textures::bind(textureId id, uint32_t slot)
    {
        textureInfo *temp = texturesData->getComponent(id);
        if(!temp)
            return;

        if(temp->needToRebuild)
            rebuild(temp);
        
        GL_CALL(context, BindTextureUnit(slot, temp->renderId));  
    }

    void textures::rebuild(textureId id)
    {
        ZoneScopedN("texture rebuild");

        textureInfo *temp = texturesData->getComponent(id);
        if(!temp)
            return;

        if(temp->needToRebuild)
            rebuild(temp);
    }
    
    void textures::rebuild(textureInfo *tex)
    {
        if(tex->renderId)
            GL_CALL(context, DeleteTextures(1, &tex->renderId));

        
        if (tex->samples > 1)
        {
            GL_CALL(context, CreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &tex->renderId));
            GL_CALL(context, TextureStorage2DMultisample(tex->renderId, tex->samples, textureFormatToOpenGlFormat(tex->format), tex->width, tex->height, GL_FALSE));
        }
        else
        {
            GL_CALL(context, CreateTextures(GL_TEXTURE_2D, 1, &tex->renderId));


            GL_CALL(context, TextureParameteri(tex->renderId , GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CALL(context, TextureParameteri(tex->renderId , GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GL_CALL(context, TextureParameteri(tex->renderId , GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
            GL_CALL(context, TextureParameteri(tex->renderId , GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CALL(context, TextureParameteri(tex->renderId , GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            GL_CALL(context, TextureStorage2D(tex->renderId, 1, textureFormatToOpenGlFormat(tex->format), tex->width, tex->height));
            if(tex->pixels != nullptr){
                ZoneScopedN("load texture data");
                tex->channels = textureFormatToChannelsCount(tex->temp.format);
                
                GL_CALL(context, 
                TextureSubImage2D(tex->renderId, 0, tex->temp.x, tex->temp.y, tex->temp.width, tex->temp.height, 
                            textureFormatToOpenGlDataFormat(tex->temp.format), tex->temp.type, tex->temp.pixels));  

                            
            }
        }
        
        tex->needToRebuild = false;    
    }


    uint32_t textures::textureFormatToOpenGlFormat(textureFormat formatToConvert)
    {
        switch (formatToConvert)
        {
            case(textureFormat::None): 				return 0;	break; 

            case(textureFormat::RGBA8): 			return GL_RGBA8;	break; 
            case(textureFormat::RED_INTEGER): 		return GL_R32I;	break; 
            case(textureFormat::RGB8): 				return GL_RGB8;	break; 
            case(textureFormat::RG8): 				return GL_RG8;	break; 
            case(textureFormat::R8): 				return GL_R8;	break; 

            case(textureFormat::DEPTH24STENCIL8): 	return GL_DEPTH24_STENCIL8;	break; 
        }

        return 0;
    }


    uint32_t textures::textureFormatToOpenGlDataFormat(textureFormat formatToConvert)
    {
        switch (formatToConvert)
        {
            case(textureFormat::None): 				return 0;	break; 

            case(textureFormat::RGBA8): 			return GL_RGBA;	break; 
            case(textureFormat::RED_INTEGER): 		return GL_RED_INTEGER;	break; 
            case(textureFormat::RGB8): 				return GL_RGB;	break; 
            case(textureFormat::RG8): 				return GL_RG;	break; 
            case(textureFormat::R8): 				return GL_R;	break; 

            case(textureFormat::DEPTH24STENCIL8): 	return GL_DEPTH24_STENCIL8;	break; 
        }

        return 0;
    }

    uint32_t textures::textureFormatToChannelsCount(textureFormat formatToConvert)
    {
        switch (formatToConvert)
        {
            case(textureFormat::None): 				return 0;	break; 

            case(textureFormat::RGBA8): 			return 4;	break; 
            case(textureFormat::RED_INTEGER): 		return 1;	break; 
            case(textureFormat::RGB8): 				return 3;	break; 
            case(textureFormat::RG8): 				return 2;	break; 
            case(textureFormat::R8): 				return 1;	break; 

            case(textureFormat::DEPTH24STENCIL8): 	return 4;	break; 
        }

        return 0;
    }
}