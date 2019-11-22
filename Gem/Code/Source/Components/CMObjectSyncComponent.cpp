// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include <regex>

#include <AzCore/JSON/rapidjson.h>
#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/JSON/stringbuffer.h>
#include <AzCore/JSON/writer.h>
#include <AzCore/JSON/prettywriter.h>

#include <LmbrCentral/Rendering/MeshComponentBus.h>

#include <AzCore/Component/Entity.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include "CMObjectSyncComponent.h"

#include <PLY/PLYObjectSyncDataStringBus.h>
#include <PLY/PLYObjectSyncSaveLoadBus.h>

#include <AzFramework/Physics/RigidBodyBus.h>

#include "CMLog.h"

using namespace CaelumMagnaVR;

CMObjectSyncComponent::CMObjectSyncComponent()
{
	

}

CMObjectSyncComponent::~CMObjectSyncComponent()
{
}

void CMObjectSyncComponent::Reflect(AZ::ReflectContext* context)
{
	AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
	if (serialize)
	{
		// Reflect the class fields that you want to serialize.
		// Base classes with serialized data should be listed as additional template
		// arguments to the Class< T, ... >() function.
		serialize->Class<CMObjectSyncComponent, AZ::Component>()
			->Version(1)
			;

		AZ::EditContext* edit = serialize->GetEditContext();
		if (edit)
		{
			edit->Class<CMObjectSyncComponent>("CM Object Serialiser", "Provides serialisation for objects to PLYObjectSyncComponent")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
				->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
				;
		}
	}
}

void CaelumMagnaVR::CMObjectSyncComponent::SetPropertiesFromDataString(std::string dataString)
{

	//Get transform (position, rotation) as well as physics properties from JSON in data string.
	TransformAsVecs tv = ParseJSONDataString(dataString);

	//Apply the transform changes to the object.
	AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetLocalTranslation, tv.translation);
	AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetLocalRotation, tv.rotation);
	AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetLocalScale, tv.scale);

	//Apply physics properties to the object.
	Physics::RigidBody *rb = nullptr;
	Physics::RigidBodyRequestBus::EventResult(rb, GetEntityId(), &Physics::RigidBodyRequestBus::Events::GetRigidBody);
	if (rb != nullptr)
	{
		AZ::Transform t;
		AZ::TransformBus::EventResult(t, GetEntityId(), &AZ::TransformBus::Events::GetLocalTM);

		//Update physics transform to match current object transform.
		//This is essential or PhysX will pull the object back to the old position.
		rb->SetTransform(t);

		rb->SetAngularVelocity(tv.angVel);

		rb->SetLinearVelocity(tv.vel);
	}

}

std::string CaelumMagnaVR::CMObjectSyncComponent::GetDataString()
{

	//Create a JSON string containing entity transform and physics information.

	using namespace rapidjson;

	Document d;

	StringBuffer buffer;
	
	AZ::Vector3 v;

	AZ::TransformBus::EventResult(v, GetEntityId(), &AZ::TransformBus::Events::GetLocalTranslation);

	Pointer("/Transform/Translation/X").Set(d, v.GetX());
	Pointer("/Transform/Translation/Y").Set(d, v.GetY());
	Pointer("/Transform/Translation/Z").Set(d, v.GetZ());

	AZ::TransformBus::EventResult(v, GetEntityId(), &AZ::TransformBus::Events::GetLocalRotation);
	
	Pointer("/Transform/Rotation/X").Set(d, v.GetX());
	Pointer("/Transform/Rotation/Y").Set(d, v.GetY());
	Pointer("/Transform/Rotation/Z").Set(d, v.GetZ());

	AZ::TransformBus::EventResult(v, GetEntityId(), &AZ::TransformBus::Events::GetLocalScale);

	Pointer("/Transform/Scale/X").Set(d, v.GetX());
	Pointer("/Transform/Scale/Y").Set(d, v.GetY());
	Pointer("/Transform/Scale/Z").Set(d, v.GetZ());

	Physics::RigidBody *rb = nullptr;
	Physics::RigidBodyRequestBus::EventResult(rb, GetEntityId(), &Physics::RigidBodyRequestBus::Events::GetRigidBody);
	if (rb != nullptr)
	{
		v = rb->GetAngularVelocity();

		Pointer("/Physics/AngVel/X").Set(d, v.GetX());
		Pointer("/Physics/AngVel/Y").Set(d, v.GetY());
		Pointer("/Physics/AngVel/Z").Set(d, v.GetZ());

		v = rb->GetLinearVelocity();

		Pointer("/Physics/Vel/X").Set(d, v.GetX());
		Pointer("/Physics/Vel/Y").Set(d, v.GetY());
		Pointer("/Physics/Vel/Z").Set(d, v.GetZ());
	}

	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	const char* data = buffer.GetString();

	return data;
}

void CaelumMagnaVR::CMObjectSyncComponent::SetObjectVisible()
{
	//Set object visible.
	LmbrCentral::MeshComponentRequestBus::Event(GetEntityId(), &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, true);
}

void CaelumMagnaVR::CMObjectSyncComponent::SetObjectInvisible()
{
	//Set object invisible.
	LmbrCentral::MeshComponentRequestBus::Event(GetEntityId(), &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, false);
}

void CaelumMagnaVR::CMObjectSyncComponent::Reset()
{

	//Reset object back to its original position.

	AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetLocalTM, m_initialTransform);

	Physics::RigidBody *rb = nullptr;
	Physics::RigidBodyRequestBus::EventResult(rb, GetEntityId(), &Physics::RigidBodyRequestBus::Events::GetRigidBody);

	if (rb != nullptr)
	{
		//Update physics transform to match current object transform.
		//This is essential or PhysX will pull the object back to the old position.
		rb->SetTransform(m_initialTransform);

		rb->SetAngularVelocity(AZ::Vector3(0,0,0));

		rb->SetLinearVelocity(AZ::Vector3(0,0,0));
	}
}

CaelumMagnaVR::CMObjectSyncComponent::TransformAsVecs CaelumMagnaVR::CMObjectSyncComponent::ParseJSONDataString(std::string dataString)
{
	TransformAsVecs tv;

	using namespace rapidjson;

	Document d;

	try
	{
		d.Parse(dataString.c_str());
	}
	catch (const std::exception &e)
	{
		CMLOG(CMLog::PLY_ERROR, "Bad JSON data. Error: " + AZStd::string(e.what()));
	}

	Value *val;

	AZ::Vector3 vec;

	//TRANSLATION

	vec.SetX(0);
	vec.SetY(0);
	vec.SetZ(0);

	val = Pointer("/Transform/Translation/X").Get(d);

	if (val != nullptr) vec.SetX(val->GetDouble());

	val = Pointer("/Transform/Translation/Y").Get(d);

	if (val != nullptr) vec.SetY(val->GetDouble());

	val = Pointer("/Transform/Translation/Z").Get(d);

	if (val != nullptr) vec.SetZ(val->GetDouble());

	tv.translation = vec;

	//ROTATION

	vec.SetX(0);
	vec.SetY(0);
	vec.SetZ(0);

	val = Pointer("/Transform/Rotation/X").Get(d);

	if (val != nullptr) vec.SetX(val->GetDouble());

	val = Pointer("/Transform/Rotation/Y").Get(d);

	if (val != nullptr) vec.SetY(val->GetDouble());

	val = Pointer("/Transform/Rotation/Z").Get(d);

	if (val != nullptr) vec.SetZ(val->GetDouble());

	tv.rotation = vec;

	//SCALE

	vec.SetX(1);
	vec.SetY(1);
	vec.SetZ(1);

	val = Pointer("/Transform/Scale/X").Get(d);

	if (val != nullptr) vec.SetX(val->GetDouble());

	val = Pointer("/Transform/Scale/Y").Get(d);

	if (val != nullptr) vec.SetY(val->GetDouble());

	val = Pointer("/Transform/Scale/Z").Get(d);

	if (val != nullptr) vec.SetZ(val->GetDouble());

	tv.scale = vec;

	//ANGLULAR VELOCITY
	
	vec.SetX(0);
	vec.SetY(0);
	vec.SetZ(0);

	val = Pointer("/Physics/AngVel/X").Get(d);

	if (val != nullptr) vec.SetX(val->GetDouble());

	val = Pointer("/Physics/AngVel/Y").Get(d);

	if (val != nullptr) vec.SetY(val->GetDouble());

	val = Pointer("/Physics/AngVel/Z").Get(d);

	if (val != nullptr) vec.SetZ(val->GetDouble());

	tv.angVel = vec;

	//VELOCITY

	vec.SetX(0);
	vec.SetY(0);
	vec.SetZ(0);

	val = Pointer("/Physics/Vel/X").Get(d);

	if (val != nullptr) vec.SetX(val->GetDouble());

	val = Pointer("/Physics/Vel/Y").Get(d);

	if (val != nullptr) vec.SetY(val->GetDouble());

	val = Pointer("/Physics/Vel/Z").Get(d);

	if (val != nullptr) vec.SetZ(val->GetDouble());
	
	tv.vel = vec;

	return tv;
}

void CMObjectSyncComponent::Init()
{
	
}

void CMObjectSyncComponent::Activate()
{
	//Store the initial transform on startup. Used to reset the object position later.
	AZ::TransformBus::EventResult(m_initialTransform, GetEntityId(), &AZ::TransformBus::Events::GetLocalTM);

	AZ::TickBus::Handler::BusConnect();
	PLY::PLYObjectSyncDataStringBus::Handler::BusConnect(GetEntityId());
}

void CMObjectSyncComponent::Deactivate()
{
	PLY::PLYObjectSyncDataStringBus::Handler::BusDisconnect();
	AZ::TickBus::Handler::BusDisconnect();
}

void CMObjectSyncComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{
	
}
