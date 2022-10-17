#include "textureManger.hpp"


void textureManger::rebuild(textureId id)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    textureInfo& temp = textures[idToIndex[id.index].index];
    if(temp.needToRebuild)
        rebuild(temp);

    temp.needToRebuild = false;
    void *pixels = malloc(temp.width * temp.hight * 4);
    GL_CALL(context, GetTextureImage(temp.renderId, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp.width * temp.hight * 4, pixels));  
}


void textureManger::rebuild(textureInfo& tex)
{
    if(tex.renderId)
        GL_CALL(context, DeleteTextures(1, &tex.renderId));

    
    if (tex.samples > 1)
    {
        GL_CALL(context, CreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &tex.renderId));
        GL_CALL(context, TextureStorage2DMultisample(tex.renderId, tex.samples, textureFormatToOpenGlFormat(tex.format), tex.width, tex.hight, GL_FALSE));
    }
    else
    {
        GL_CALL(context, CreateTextures(GL_TEXTURE_2D, 1, &tex.renderId));


        GL_CALL(context, TextureParameteri(tex.renderId , GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(context, TextureParameteri(tex.renderId , GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(context, TextureParameteri(tex.renderId , GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CALL(context, TextureParameteri(tex.renderId , GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(context, TextureParameteri(tex.renderId , GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        GL_CALL(context, TextureStorage2D(tex.renderId, 1, textureFormatToOpenGlFormat(tex.format), tex.width, tex.hight));
        if(tex.pixels != nullptr){
            
            GL_CALL(context, TextureSubImage2D(tex.renderId, 0, tex.temp.x, tex.temp.y, tex.temp.width, tex.temp.hight, textureFormatToOpenGlDataFormat(tex.temp.format), tex.temp.type, tex.temp.pixels));  
            
            GL_CALL(context, GetTextureImage(tex.renderId, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.width * tex.hight * 4, tex.temp.pixels)); 
        }
    }
}

void textureManger::handleRequsets()
{
    while (!toLoad.empty())
    {
        loadTextureRequst temp = toLoad.front();
        toLoad.pop();
        if(idToIndex[temp.id.index].gen != temp.id.gen)
            continue;

        textureInfo& tempInfo = textures[idToIndex[temp.id.index].index];
        if(tempInfo.needToRebuild)
            rebuild(tempInfo);
            
        tempInfo.needToRebuild = false;
        tempInfo.temp = temp;
        tempInfo.pixels = temp.pixels;

        GL_CALL(context, TextureSubImage2D(tempInfo.renderId, 0, temp.x, temp.y, temp.width, temp.hight, textureFormatToOpenGlDataFormat(temp.format), temp.type, temp.pixels));  
    }

    while (!toDelete.empty())
    {
        GL_CALL(context, DeleteTextures(1, &toDelete.front()));  
        toDelete.pop();
    }
    
}


void textureManger::bind(textureId texId, uint32_t slot)
{
    if(idToIndex[texId.index].gen != texId.gen)
        return;

    textureInfo& temp = textures[idToIndex[texId.index].index];
    if(temp.needToRebuild)
        rebuild(temp);
    
    temp.needToRebuild = false;

    GL_CALL(context, BindTextureUnit(slot, temp.renderId));  

}

uint32_t textureManger::getRenderId(textureId texId)
{
    if(idToIndex[texId.index].gen != texId.gen)
        return (uint32_t)-1;

    textureInfo& temp = textures[idToIndex[texId.index].index];
    return temp.renderId;
}

void textureManger::setRenderId(textureId id, uint32_t renderId)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    textureInfo& temp = textures[idToIndex[id.index].index];
    temp.renderId = renderId;
}

textureId textureManger::createTexture(textureFormat format, uint32_t width, uint32_t hight)
{
    textureId id;
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
        idToIndex.push_back({0, (uint32_t)textures.size()});
    }

    textureInfo info;
    info.id = id;
    info.needToRebuild = true;
    info.width = width;
    info.hight = hight;
    info.format = format;
    
    idToIndex[id.index].index = textures.size();
    textures.push_back(info);
    return id;

}


void textureManger::resize(textureId texId, uint32_t width, uint32_t hight)
{
    if(idToIndex[texId.index].gen != texId.gen)
        return;

    textureInfo& temp = textures[idToIndex[texId.index].index];
    temp.width = width;
    temp.hight = hight;
    temp.needToRebuild = true;

}

void textureManger::loadBuffer(textureId id, uint32_t x, uint32_t y, uint32_t width, uint32_t hight, textureFormat format, uint32_t type, void* pixels)
{
    if(idToIndex[id.index].gen != id.gen)
        return;

    textureInfo& temp = textures[idToIndex[id.index].index];
    toLoad.push({
        id, x, y, width, hight, format, type, pixels
    });

    temp.needToRebuild = true;
}


void textureManger::deleteTexture(textureId texId)
{
    if(texId.gen != idToIndex[texId.index].gen)
        return;

    textureInfo temp = textures[idToIndex[texId.index].index];
    
    toDelete.push(temp.renderId);
    delete[] temp.pixels;

    idToIndex[texId.index].gen++;
    
    textureInfo last = textures[textures.size() - 1];
    textures[idToIndex[texId.index].index] = last;
    idToIndex[last.id.index].index = idToIndex[texId.index].index;

    textures.pop_back();
    idToIndex[texId.index].index = -1;
    freeSlots.push_back(texId.index);
}

uint32_t textureManger::getWidth(textureId texId)
{
    if(idToIndex[texId.index].gen != texId.gen)
        return - 1;

    textureInfo& temp = textures[idToIndex[texId.index].index];
    return temp.width;
}

uint32_t textureManger::getHight(textureId texId)
{
    if(idToIndex[texId.index].gen != texId.gen)
        return -1;

    textureInfo& temp = textures[idToIndex[texId.index].index];
    return temp.hight;
}

uint32_t textureManger::textureFormatToOpenGlFormat(textureFormat formatToConvert)
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


uint32_t textureManger::textureFormatToOpenGlDataFormat(textureFormat formatToConvert)
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