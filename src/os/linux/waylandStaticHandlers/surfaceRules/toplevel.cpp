#include "toplevel.hpp"
#include "cpuRendering.hpp"
#include "linuxWindowAPI.hpp"
#include "openGLRendering.hpp"

#include <glad/gl.h>
#include <EGL/egl.h>

#include <stb_image.h>




void toplevel::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    ZoneScoped;

    if(width == 0 || height == 0)
        return;

    surfaceId id = *(surfaceId*)data;
    uint8_t index = surface::idToIndex[id.index].surfaceDataIndex;
    if(index == (uint8_t)-1 || id.gen != idToIndex[id.index].gen)
        return;

    surface::surfaceData& temp = surface::surfaces[index];
    windowSizeState activeState = windowSizeState::undefined;

    for (size_t i = 0; i < states->size; i+=4)
    {
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_RESIZING )
            activeState = windowSizeState::reizing;

        if (((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_ACTIVATED && temp.height != height && temp.width != width)
            activeState = windowSizeState::reizing;

        
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_FULLSCREEN)
            activeState = windowSizeState::fullscreen;
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_MAXIMIZED)
            activeState = windowSizeState::maximized;
        
    }    
    if(activeState == windowSizeState::undefined)
        return;
    temp.height = height;
    temp.width = width;
    LOG_INFO(width << ", " << height)
    index = openGLRendering::idToIndex[id.index].renderDataIndex;
    openGLRendering::renderInfo& tempR = openGLRendering::surfacesToRender[index];

    wl_egl_window_resize (tempR.eglWindow, width, height, 0, 0);

    // cpuRendering::reallocateWindowCpuPool(id);

    index = idToIndex[id.index].resizeEventIndex;
    if(index != (uint8_t)-1)
        std::thread(resizeEventListeners[index], windowResizeData{height, width, activeState}).detach();
}

void toplevel::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    ZoneScoped;

    surfaceId id = *(surfaceId*)data;

    uint8_t index = idToIndex[id.index].closeEventIndex;
    if(index != (uint8_t)-1 || id.gen == idToIndex[id.index].gen)
        closeEventListeners[index]();
        
    linuxWindowAPI::closeWindow(surface::surfaces[surface::idToIndex[id.index].surfaceDataIndex].parentWindowId);
}

void toplevel::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    ZoneScoped;

    surfaceId id = *(surfaceId*)data;
    uint8_t index = surface::idToIndex[id.index].surfaceDataIndex;
    if(index == (uint8_t)-1 || id.gen != idToIndex[id.index].gen)
        return;


    xdg_surface_ack_configure(xdg_surface, serial);

    index = openGLRendering::idToIndex[id.index].renderDataIndex;
    openGLRendering::renderInfo& temp = openGLRendering::surfacesToRender[index];


    // struct wl_buffer *buffer = cpuRendering::allocateWindowBuffer(id, tempR.bufferToRender);
    
    // int tempBuffer = tempR.bufferToRender;
    // tempR.bufferToRender = tempR.bufferInRender;
    // tempR.bufferInRender = tempBuffer;

    // wl_surface_attach(surface::getSurface(id), buffer, 0, 0);
    if(temp.eglWindow == NULL){
        
        temp.eglWindow = wl_egl_window_create (surface::getSurface(id), surface::getWindowWidth(id), surface::getWindowHeight(id));
        temp.eglSurface = eglCreateWindowSurface (openGLRendering::context->eglDisplay, openGLRendering::context->eglConfig, (EGLNativeWindowType)temp.eglWindow, NULL);
        
        openGLRendering::context->makeCurrent(temp.eglSurface, temp.eglSurface);

        GL_CALL(openGLRendering::context, PixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(openGLRendering::context, GenTextures(1, &temp.textureBufferId));  
        GL_CALL(openGLRendering::context, BindTexture(GL_TEXTURE_2D, temp.textureBufferId));  

    
        GL_CALL(openGLRendering::context, TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	
        GL_CALL(openGLRendering::context, TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL_CALL(openGLRendering::context, TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CALL(openGLRendering::context, TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    
        uint8_t *data = new uint8_t[surface::getWindowWidth(id) * surface::getWindowHeight(id) * 4];

        for (size_t i = 0; i < surface::getWindowWidth(temp.id) * surface::getWindowHeight(temp.id) * 4; i += 4)
        {
            data[i + 0] = 0xFF;
            data[i + 1] = 0x10;
            data[i + 2] = 0xFF;
            data[i + 3] = 0xFF;
        }

        temp.bufferInRenderTex = openGLRendering::renderer->textures->createTexture(textureFormat::RGBA8, surface::getWindowWidth(id), surface::getWindowHeight(id));
        temp.bufferToRenderTex = openGLRendering::renderer->textures->createTexture(textureFormat::RGBA8, surface::getWindowWidth(id), surface::getWindowHeight(id));
        temp.freeBufferTex = openGLRendering::renderer->textures->createTexture(textureFormat::RGBA8, surface::getWindowWidth(id), surface::getWindowHeight(id));
        
        openGLRendering::renderer->textures->loadBuffer(temp.bufferInRenderTex, 0, 0, surface::getWindowWidth(id), surface::getWindowHeight(id), textureFormat::RGBA8, GL_UNSIGNED_BYTE, data);


        temp.bufferInRender = openGLRendering::renderer->frameBuffers->createFrameBuffer(surface::getWindowWidth(id), surface::getWindowHeight(id));
        temp.bufferToRender = openGLRendering::renderer->frameBuffers->createFrameBuffer(surface::getWindowWidth(id), surface::getWindowHeight(id));
        temp.freeBuffer = openGLRendering::renderer->frameBuffers->createFrameBuffer(surface::getWindowWidth(id), surface::getWindowHeight(id));
        
        openGLRendering::renderer->frameBuffers->attachColorRenderTarget(temp.bufferInRender, temp.bufferInRenderTex, 0);
        openGLRendering::renderer->frameBuffers->attachColorRenderTarget(temp.bufferToRender, temp.bufferToRenderTex, 0);
        openGLRendering::renderer->frameBuffers->attachColorRenderTarget(temp.freeBuffer, temp.freeBufferTex, 0);

        openGLRendering::renderer->renderRequest({
            temp.bufferInRender, { {{255, 16777215}, {255, 16777215}, {[0 ... 31] = {255, 16777215}}, renderMode::triangles} }
        });
        openGLRendering::renderer->renderRequest({
            temp.bufferInRender, { {{255, 16777215}, {255, 16777215}, {[0 ... 31] = {255, 16777215}}, renderMode::triangles} }
        });
        openGLRendering::renderer->renderRequest({
            temp.bufferInRender, { {{255, 16777215}, {255, 16777215}, {[0 ... 31] = {255, 16777215}}, renderMode::triangles} }
        });
        
        openGLRendering::context->swapBuffers(temp.eglSurface);
        wl_surface_commit(surface::getSurface(id));
    }

        openGLRendering::renderer->renderRequest({
            temp.bufferInRender, { {{255, 16777215}, {255, 16777215}, {[0 ... 31] = {255, 16777215}}, renderMode::triangles} }
        });
}




void toplevel::setCloseEventListener(surfaceId winId, std::function<void()> callback)
{
    uint32_t index = idToIndex[winId.index].closeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        closeEventListeners[index] = callback;
        closeEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].closeEventIndex = closeEventListeners.size();
    closeEventListeners.push_back(callback);
    closeEventId.push_back(winId);
}

void toplevel::setResizeEventListener(surfaceId winId, std::function<void(const windowResizeData&)> callback)
{
    uint32_t index = idToIndex[winId.index].resizeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        resizeEventListeners[index] = callback;
        resizeEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].resizeEventIndex = resizeEventListeners.size();
    resizeEventListeners.push_back(callback);
    resizeEventId.push_back(winId);
}



void toplevel::unsetCloseEventListener(surfaceId winId)
{        
    uint32_t index = idToIndex[winId.index].closeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = closeEventListeners.size() - 1;
    idToIndex[closeEventId[lastIndex].index].closeEventIndex = index;
    closeEventListeners[index] = closeEventListeners[lastIndex];
    closeEventId[index] = closeEventId[lastIndex];

    closeEventListeners.pop_back();
    closeEventId.pop_back();

    idToIndex[winId.index].closeEventIndex = -1;
}  

void toplevel::unsetResizeEventListener(surfaceId winId)
{        
    uint32_t index = idToIndex[winId.index].resizeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = resizeEventListeners.size() - 1;
    idToIndex[resizeEventId[lastIndex].index].resizeEventIndex = index;
    resizeEventListeners[index] = resizeEventListeners[lastIndex];
    resizeEventId[index] = resizeEventId[lastIndex];

    resizeEventListeners.pop_back();
    resizeEventId.pop_back();

    idToIndex[winId.index].resizeEventIndex = -1;
}

void toplevel::setWindowTitle(surfaceId id, const std::string& title)
{        
    if(idToIndex[id.index].toplevelDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen){
        xdg_toplevel_set_title(topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].xdgToplevel, topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title.c_str());
        topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title = title;
    }
}


std::string toplevel::getWindowTitle(surfaceId id)
{
    if(idToIndex[id.index].toplevelDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        return topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title;

    return "error window with id: " + std::to_string((id.gen << 8) + id.index) + " was'nt found";
}

void toplevel::allocateTopLevel(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    toplevelSurfaceInfo info;
    info.title = surfaceData.title;
    info.id = id;

    info.xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, s);
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener, new surfaceId(id));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener, new surfaceId(id));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    xdg_toplevel_set_app_id(info.xdgToplevel, "new tale game engine");
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecorationListener, new surfaceId(id));
    
    idToIndex[id.index].gen = id.gen;
    idToIndex[id.index].toplevelDataIndex = topLevelSurfaces.size();
    topLevelSurfaces.push_back(info);
}

void toplevel::deallocateTopLevel(surfaceId winId)
{
    uint8_t index = idToIndex[winId.index].toplevelDataIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == (uint8_t)-1)
        return;


    toplevelSurfaceInfo& temp = topLevelSurfaces[index];

    zxdg_toplevel_decoration_v1_destroy(temp.topLevelDecoration);
    xdg_toplevel_destroy(temp.xdgToplevel);
    xdg_surface_destroy(temp.xdgSurface);

    unsetCloseEventListener(winId);
    unsetResizeEventListener(winId);
}
