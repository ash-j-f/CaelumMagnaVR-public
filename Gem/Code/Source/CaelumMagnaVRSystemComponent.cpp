// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include <CaelumMagnaVRSystemComponent.h>

#include "Console.h"

namespace CaelumMagnaVR
{
	CaelumMagnaVRSystemComponent::CaelumMagnaVRSystemComponent()
		: m_registeredConsoleCommands(false)
	{
	
	}

	CaelumMagnaVRSystemComponent::~CaelumMagnaVRSystemComponent()
	{
	}

	void CaelumMagnaVRSystemComponent::Reflect(AZ::ReflectContext* context)
    {
		AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
        if (serialize)
        {
            serialize->Class<CaelumMagnaVRSystemComponent, AZ::Component>()
                ->Version(0)
                ;

			AZ::EditContext* edit = serialize->GetEditContext();
            if (edit)
            {
                edit->Class<CaelumMagnaVRSystemComponent>("CaelumMagnaVR", "Caelum Magna VR Starmap")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void CaelumMagnaVRSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("CaelumMagnaVRService"));
    }

    void CaelumMagnaVRSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("CaelumMagnaVRService"));
    }

    void CaelumMagnaVRSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        AZ_UNUSED(required);
    }

    void CaelumMagnaVRSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        AZ_UNUSED(dependent);
    }

    void CaelumMagnaVRSystemComponent::Init()
    {
		AZ_Printf("Script", "%s", "CaelumMagnaVR Initialised");
    }

    void CaelumMagnaVRSystemComponent::Activate()
    {
        CaelumMagnaVRRequestBus::Handler::BusConnect();
		AZ::TickBus::Handler::BusConnect();

		AZ_Printf("Script", "%s", "CaelumMagnaVR Activated");
    }

    void CaelumMagnaVRSystemComponent::Deactivate()
    {
        CaelumMagnaVRRequestBus::Handler::BusDisconnect();
		AZ::TickBus::Handler::BusDisconnect();

		AZ_Printf("Script", "%s", "CaelumMagnaVR Deactivated");
    }

	void CaelumMagnaVRSystemComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
	{
		//Register console commands here. For some reason this doesn't work if placed in CaelumMagnaVRSystemComponent::Activate()
		if (!m_registeredConsoleCommands)
		{
			m_registeredConsoleCommands = true;

			//Register commands.
			m_consoleCommandManager = std::make_unique<Console>();
		}
	}
}
