// A simple console log class. Messages are printed to the Lumberyard game console.
// Derives from the Log class included with the PLY Gem.
// Designed to be used as a singleton, called using the provided macros.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <PLY/Log.hpp>

#define CMLOG(level, message) CaelumMagnaVR::CMLog::getInstance()->Print(level, message)
#define CMLOG_SET_LEVEL(level) CaelumMagnaVR::CMLog::getInstance()->SetLogLevel(level)
#define CMLOG_GET_LEVEL CaelumMagnaVR::CMLog::getInstance()->GetLogLevel()
#define CMLOG_GET_DEFAULT_LEVEL CaelumMagnaVR::CMLog::getInstance()->GetDefaultLogLevel()

namespace CaelumMagnaVR
{
	class CMLog : public PLY::Log
	{
	public:
		
		//Get the singleton instance.
		static CMLog *getInstance();
	
	private:

		CMLog();
		~CMLog();
	};
}