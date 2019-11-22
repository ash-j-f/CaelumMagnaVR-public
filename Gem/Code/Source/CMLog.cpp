// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include "CMLog.h"

using namespace CaelumMagnaVR;

CMLog::CMLog()
{

}

CMLog::~CMLog()
{

}

CMLog *CMLog::getInstance()
{
	static CMLog instance;
	return &instance;
}