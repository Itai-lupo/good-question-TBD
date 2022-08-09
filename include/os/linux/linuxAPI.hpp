#pragma once
#include "osAPI.hpp"
#include "log.hpp"
#include "linuxWindowAPI.hpp"


#include <string.h>


#include <unistd.h>


class linuxAPI: public osAPI
{
	private:
        linuxWindowAPI* windowApi;
	public:
        linuxAPI();
        virtual ~linuxAPI();

        virtual windowId createWindow(const windowSpec& windowToCreate) override;

        virtual int pollEvents() override;
        virtual bool isWindowOpen(windowId winId) override;
        virtual void setVSyncForCurrentContext(bool enabled) override;
        virtual void makeContextCurrent(windowId winId) override;
        virtual void closeWindow(windowId winId) override;
        virtual void swapBuffers(windowId winId) override;
        virtual windowId getCurrentContextWindowId() override;

        virtual void *getProcAddress() override;

};



