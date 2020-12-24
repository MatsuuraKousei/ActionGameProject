#include "Cutter.h"


void Cutter::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["Pos"].is_array())
	{
		auto& p = jsonObj["Pos"].array_items();
		m_pos = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}
}

void Cutter::Update()
{
	m_rot.y -= 0.03;

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}
