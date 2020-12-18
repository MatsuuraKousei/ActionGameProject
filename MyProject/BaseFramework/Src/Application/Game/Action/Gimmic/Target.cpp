#include "Target.h"
#include "Door.h"
#include "../../Scene.h"

void Target::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["Pos"].is_array())
	{
		auto& p = jsonObj["Pos"].array_items();
		m_pos = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	if (jsonObj["DoorNum"].is_number())
	{
		m_DoorNum = jsonObj["DoorNum"].number_value();
	}

	for (int i = 0; i < m_DoorNum; i++)
	{
		std::string string = "Door";
		string += std::to_string(i);
		if (jsonObj[string].is_array())
		{
			auto& p = jsonObj[string].array_items();
			m_DoorPos[i] = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
		}
		std::string string2 = "DoorAngle";
		string2 += std::to_string(i);
		if (jsonObj[string2].is_array())
		{
			auto& p = jsonObj[string2].array_items();
			m_DoorAngle[i] = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
		}
	}

	BornDoor();

	m_Hp = 2;
	
}

void Target::Update()
{
	if (!m_alive) { return; }

	if (m_Hp == 1)
	{
		if (m_rot.z > -1)
		{
			m_rot.z = 0;
			m_Switch = true;
			m_spDoor->Switch(m_Switch);
		}
		m_rot.z++;
	}


	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Target::BornDoor()
{
	for (int i = 0; i < m_DoorNum; i++)
	{
		m_spDoor = std::make_shared<Door>();

		m_spDoor->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Door.Json"));

		Matrix mat;

		mat.SetTranslation(m_DoorPos[i]);

		m_spDoor->m_pos = m_DoorPos[i];

		m_spDoor->m_rot = m_DoorAngle[i];

		m_spDoor->SetTag(TAG_StageObject);

		Scene::GetInstance().AddObject(m_spDoor);
	}
}
