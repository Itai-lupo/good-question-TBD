#include "subsurface.hpp"
#include "linuxWindowAPI.hpp"



void subsurface::allocateSubsurface(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    subsurfaceInfo info;
    info.id = id;
    info.x = surfaceData.x;
    info.y = surfaceData.y;

    info.subsurface = wl_subcompositor_get_subsurface(subcompositor,  s, surface::getSurface(surfaceData.parentSurface));
    wl_subsurface_set_desync(info.subsurface);
    wl_subsurface_set_position(info.subsurface, info.x, info.y);

    idToIndex.push_back({0});
    idToIndex[id.index].gen = id.gen;
    idToIndex[id.index].index = topLevelSurfaces.size();
    topLevelSurfaces.push_back(info);
}

void subsurface::deallocateTopLevel(surfaceId winId)
{
    uint8_t index = idToIndex[winId.index].index;
    if(idToIndex[winId.index].gen != winId.gen || index == (uint8_t)-1)
        return;


    subsurfaceInfo& temp = topLevelSurfaces[index];
    wl_subsurface_destroy(temp.subsurface);
}
