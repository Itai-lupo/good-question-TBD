#include "subsurface.hpp"
#include "linuxWindowAPI.hpp"

void subsurface::init(entityPool *surfacesPool)
{
    subsurfaces = new subsurfaceInfoComponenet(surfacesPool);
}

void subsurface::close()
{
    delete subsurfaces;
}


void subsurface::allocateSubsurface(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    subsurfaceInfo info;
    info.id = id;
    info.x = surfaceData.x;
    info.y = surfaceData.y;
    info.title = surfaceData.title;
    
    info.subsurface = wl_subcompositor_get_subsurface(subcompositor,  s, surface::getSurface(surfaceData.parentSurface));
    wl_subsurface_set_desync(info.subsurface);
    wl_subsurface_set_position(info.subsurface, info.x, info.y);

    subsurfaces->setComponent(id, info);
}

void subsurface::deallocateTopLevel(surfaceId winId)
{
    subsurfaces->deleteComponent(winId);
}
