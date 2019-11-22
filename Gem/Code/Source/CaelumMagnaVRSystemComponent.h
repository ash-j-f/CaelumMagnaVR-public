// Core Lumberyard system component for the project.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <AzCore/Component/Component.h>

#include <CaelumMagnaVR/CaelumMagnaVRBus.h>

#include <AzCore/Component/TickBus.h>

namespace CaelumMagnaVR
{
	//Forward declarations.
	class Console;

    class CaelumMagnaVRSystemComponent
        : public AZ::Component,
        protected CaelumMagnaVRRequestBus::Handler,
		public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(CaelumMagnaVRSystemComponent, "{48D12C8E-F173-40D0-8381-0974E032AD0F}");

		CaelumMagnaVRSystemComponent();
		~CaelumMagnaVRSystemComponent();

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:

        ////////////////////////////////////////////////////////////////////////
        // CaelumMagnaVRRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

	private:

		//Tick handler.
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		//Have console commands been registered?
		bool m_registeredConsoleCommands;

		//Console command manager.
		std::unique_ptr<Console> m_consoleCommandManager;
    };
}
