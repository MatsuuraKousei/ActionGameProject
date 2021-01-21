#include "Righting.h"

void Righting::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["col"].is_number())
	{
		float col = jsonObj["col"].number_value();

		m_RightCol = col;
	}

	if (jsonObj["Color"].is_array())
	{
		auto& p = jsonObj["Color"].array_items();
		m_color = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}
}

void Righting::Update()
{
	Vector3 pos = { 0,0,0 };
	pos += m_mWorld.GetTranslation();
	SHADER.AddPointLight(pos, m_RightCol, m_color);
}
