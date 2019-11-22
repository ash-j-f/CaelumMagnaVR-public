// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include "Console.h"

#include <algorithm>
#include <cctype>
#include <string>

#include <ISystem.h>
#include <IConsole.h>

#include <platform_impl.h>

#include <Include/CaelumMagnaVR/StarMapComponentBus.h>
#include <Include/CaelumMagnaVR/SyncDemoComponentBus.h>

using namespace CaelumMagnaVR;

Console::Console()
{
	//Register console commands.
	ISystem* system = nullptr;
	CrySystemRequestBus::BroadcastResult(system, &CrySystemRequestBus::Events::GetCrySystem);
	if (system)
	{
		IConsole* console = system->GetIConsole();
		console->AddCommand("cm", &ConsoleCommand);
	}
	else
	{
		AZ_Printf("CaelumMagnaVR", "%s","ERROR: Unable to get CrySystem pointer when registering console commands.");
	}
}

Console::~Console()
{

}

void Console::ConsoleCommand(IConsoleCmdArgs *cmdArgs)
{

	//Process console command.
	if (cmdArgs->GetArgCount() > 1)
	{
		int argCount = cmdArgs->GetArgCount();
		const char* command1 = cmdArgs->GetArg(1);
		AZStd::string c1 = AZStd::string(command1);

		//Convert argument to lowercase
		std::transform(c1.begin(), c1.end(), c1.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (c1 == "start")
		{
			//DISABLED. Star map scene now starts automatically.
			//StarMapComponentRequestBus::Broadcast(&StarMapComponentRequestBus::Events::GetStars);
		}
		else if (c1 == "reset")
		{
			//Reset the object to database sync demo scene.
			SyncDemoComponentBus::Broadcast(&SyncDemoComponentBus::Events::Reset);
		}
		else
		{
			AZ_Printf("CaelumMagnaVR", "%s", "Unknown CM command");
		}
	}
	else
	{
		AZ_Printf("CaelumMagnaVR", "%s", "Bad CM command");
	}
}