#pragma once
#include "core.hpp"

#include <string>
#include <functional>
#include <vector>
#include <glm/glm.hpp>

struct windowSpec
{
	windowSpec(std::string title, int w, int h): w(w), h(h), title(title){}
	int w, h;
	std::string title;
};


class osAPI
{
	public:
		osAPI();
		~osAPI();
		windowId createWindow(const windowSpec& windowToCreate);
		
        int pollEvents();
        bool isWindowOpen(windowId winId);
        void setVSyncForCurrentContext(bool enabled);
        void makeContextCurrent(windowId winId);
        void closeWindow(windowId winId);
        void swapBuffers(windowId winId);
        windowId getCurrentContextWindowId();

        void *getProcAddress();

};

