// A class to update the size and position of boundary indicator elements in the star map.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <vector>
#include <list>

#include <mutex>

#include <AzCore/Math/Vector3.h>

#include <AzCore/Component/Component.h>

#include <CaelumMagnaVR/StarMapComponentBus.h>

#include <AzCore/Component/TickBus.h>

namespace CaelumMagnaVR
{

	class StarMapBoundaryScalerComponent
		: public AZ::Component,
		public AZ::TickBus::Handler
	{

	public:

		StarMapBoundaryScalerComponent();
		~StarMapBoundaryScalerComponent();

		//UUID crested using VS > Tools > Create GUID. This can be any GUID unique to the project.
		AZ_COMPONENT(StarMapBoundaryScalerComponent, "{8E57F760-1152-401C-9A4E-EBC5409FEE04}", AZ::Component);

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

	protected:

		// AZ::Component interface implementation.
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// Optional functions for defining provided and dependent services.
		/*
		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		*/

	private:

		//Boundary scale on the last tick.
		double m_lastScale;

		//Tick order definition. This value sets where in global tick order this component is called.
		//TICK_PLACEMENT is fairly early in the tick order.
		//TICK_DEFAULT is the default position for components.
		//TICK_GAME is recommended for "game related components".
		inline int GetTickOrder() override { return AZ::TICK_GAME; };

		//Tick handler.
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
	};
}