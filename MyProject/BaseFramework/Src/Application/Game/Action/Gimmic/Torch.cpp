#include "Torch.h"

void Torch::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);
}

void Torch::Update()
{
	if (!m_alive) { return; }

	Vector3 pos = { 0,1,0 };
	pos += m_mWorld.GetTranslation();
	SHADER.AddPointLight(pos, 50, { 0.1f,0.1f,0.1f });
}
