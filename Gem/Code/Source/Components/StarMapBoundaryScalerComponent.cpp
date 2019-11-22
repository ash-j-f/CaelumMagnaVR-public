// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Vector3.h>

#include <Components/StarMapBoundaryScalerComponent.h>

using namespace CaelumMagnaVR;

StarMapBoundaryScalerComponent::StarMapBoundaryScalerComponent() :
	m_lastScale(1)
{	

}

StarMapBoundaryScalerComponent::~StarMapBoundaryScalerComponent()
{
}

void StarMapBoundaryScalerComponent::Reflect(AZ::ReflectContext* context)
{
	AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
	if (serialize)
	{
		// Reflect the class fields that you want to serialize.
		// In this example, m_runtimeStateNoSerialize is not reflected for serialization.
		// Base classes with serialized data should be listed as additional template
		// arguments to the Class< T, ... >() function.
		serialize->Class<StarMapBoundaryScalerComponent, AZ::Component>()
			->Version(1)
			;

		AZ::EditContext* edit = serialize->GetEditContext();
		if (edit)
		{
			edit->Class<StarMapBoundaryScalerComponent>("Star Map Boundary Scaler", "Boundary Scaler for the Star Map Demo")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
				;
		}
	}
}

void StarMapBoundaryScalerComponent::Init()
{
	
}

void StarMapBoundaryScalerComponent::Activate()
{
	AZ::TickBus::Handler::BusConnect();
}

void StarMapBoundaryScalerComponent::Deactivate()
{
	AZ::TickBus::Handler::BusDisconnect();
}

void StarMapBoundaryScalerComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{
	//If boundary scale has changed, update the position and size of the boundary indicator.

	double newScale = 0;
	StarMapComponentRequestBus::BroadcastResult(newScale, &StarMapComponentRequestBus::Events::GetViewAreaExtentMetres);

	if (newScale > 0 && newScale != m_lastScale)
	{
		AZStd::vector<AZ::EntityId> existingEntityIDs;
		AZ::TransformBus::EventResult(existingEntityIDs, GetEntityId(), &AZ::TransformBus::Events::GetAllDescendants);

		for (auto &eid : existingEntityIDs)
		{
			//Entities with no descendants must be a grid line.
			AZStd::vector<AZ::EntityId> descendants;
			AZ::TransformBus::EventResult(descendants, eid, &AZ::TransformBus::Events::GetAllDescendants);

			if (descendants.size() == 0)
			{
				AZ::Transform t;
				AZ::TransformBus::EventResult(t, eid, &AZ::TransformBus::Events::GetLocalTM);

				float scaleChange = static_cast<float>(newScale / m_lastScale);

				t.MultiplyByScale(AZ::Vector3(1,1,scaleChange));
				float newScaleF = static_cast<float>(newScale);
				t.SetPosition(t.GetPosition() * scaleChange);

				AZ::TransformBus::Event(eid, &AZ::TransformBus::Events::SetLocalTM, t);
			}
		}

		m_lastScale = newScale;
	}
	
}
