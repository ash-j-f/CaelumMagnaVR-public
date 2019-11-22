// A game entity component that provides serialisation and de-serialisation methods for synchronising the object with the database.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/TransformBus.h>

#include <PLY/PLYObjectSyncSaveLoadBus.h>
#include <PLY/PLYObjectSyncDataStringBus.h>

namespace CaelumMagnaVR
{

	class CMObjectSyncComponent
		: public AZ::Component, 
		public AZ::TickBus::Handler,
		public PLY::PLYObjectSyncDataStringBus::Handler
	{

	public:

		CMObjectSyncComponent();
		~CMObjectSyncComponent();

		//UUID crested using VS > Tools > Create GUID. This can be any GUID unique to the project.
		AZ_COMPONENT(CMObjectSyncComponent, "{E3044CB7-D3C9-46F1-845F-0EFAE7642562}", AZ::Component);

		//Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		//Transform and physics data as separate vectors.
		struct TransformAsVecs
		{
			AZ::Vector3 translation;
			AZ::Vector3 rotation;
			AZ::Vector3 scale;
			AZ::Vector3 angVel;
			AZ::Vector3 vel;

		};

		//Parse a JSON string to extract the transform and physics information for the object.
		//@param dataString The JSON string to parse.
		static TransformAsVecs ParseJSONDataString(std::string dataString);

	protected:

		//AZ::Component interface implementation.
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		//Update the object's transform and physics properties from data in a given JSON string.
		//@param dataString The JSON string to parse.
		void SetPropertiesFromDataString(std::string dataString) override;

		//Create a JSON String from the transform and physics properties of the object.
		std::string GetDataString() override;

		//Set the object visible. Enables the object's mesh.
		void SetObjectVisible() override;

		//Set the object invisible. Disables the object's mesh.
		void SetObjectInvisible() override;

		//Reset the object to its original position and rotation, and negate physics properties.
		void Reset() override;

		//Tick handler.
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		//Set tick order.
		inline int GetTickOrder() override { return AZ::ComponentTickBus::TICK_GAME; };

		// Optional functions for defining provided and dependent services.
		/*
		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		*/

		//The initial transform on startup. Used to reset the object position.
		AZ::Transform m_initialTransform;
	};
}