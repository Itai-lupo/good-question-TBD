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

		virtual uint64_t createWindow(windowSpec *windowToCreate) override;

        virtual int pollEvents() override;
        virtual bool isWindowOpen(uint64_t winId) override;
        virtual void setVSyncForCurrentContext(bool enabled) override;
        virtual void makeContextCurrent(uint64_t winId) override;
        virtual void closeWindow(uint64_t winId) override;
        virtual void swapBuffers(uint64_t winId) override;
        virtual uint64_t getCurrentContextWindowId() override;

        virtual void *getProcAddress() override;

};



