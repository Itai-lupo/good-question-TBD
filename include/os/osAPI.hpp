#pragma once
#include "osAPI.hpp"
#include "window.hpp"

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
		virtual uint64_t createWindow(windowSpec *windowToCreate) = 0;
		
        virtual int pollEvents() = 0;
        virtual bool isWindowOpen(uint64_t winId) = 0;
        virtual void setVSyncForCurrentContext(bool enabled) = 0;
        virtual void makeContextCurrent(uint64_t winId) = 0;
        virtual void closeWindow(uint64_t winId) = 0;
        virtual void swapBuffers(uint64_t winId) = 0;
        virtual uint64_t getCurrentContextWindowId() = 0;

        virtual void *getProcAddress() = 0;

		static osAPI *init();
};

