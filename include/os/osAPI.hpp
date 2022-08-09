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
		virtual ~osAPI() = default;
		virtual windowId createWindow(const windowSpec& windowToCreate) = 0;
		
        virtual int pollEvents() = 0;
        virtual bool isWindowOpen(windowId winId) = 0;
        virtual void setVSyncForCurrentContext(bool enabled) = 0;
        virtual void makeContextCurrent(windowId winId) = 0;
        virtual void closeWindow(windowId winId) = 0;
        virtual void swapBuffers(windowId winId) = 0;
        virtual windowId getCurrentContextWindowId() = 0;

        virtual void *getProcAddress() = 0;

		static osAPI *init();
};

