#include "SpawnManager.h"
#include "Boar.h"
#include "../Scene.h"

void SpawnManeger::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_spBoar = std::make_shared<Boar>();

	SpawnFlg = false;
}

void SpawnManeger::Update()
{
	Scene::GetInstance().AddDebugLine(m_mWorld.GetTranslation(), Math::Vector3(0.0f, 10.0f, 0.0f), Math::Vector4(1, 0, 0, 1));

	m_mWorld.SetTranslation(m_pos);

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 15;

	

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			if (SpawnFlg) { return; }
			Scene::GetInstance().AddDebugSphereLine(m_mWorld.GetTranslation(), info.m_radius, { 0.0f,0.0f,1.0f,1.0f });
			m_spBoar->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Boar.json"));

			m_spBoar->m_pos.x = m_mWorld.GetTranslation().x;
			m_spBoar->m_pos.y = m_mWorld.GetTranslation().y - 2.0f;
			m_spBoar->m_pos.z = m_mWorld.GetTranslation().z;

			Scene::GetInstance().AddObject(m_spBoar);

			SpawnFlg = true;
		}

		if (!m_spBoar->IsAlive())
		{
			SpawnFlg = false;
		}
	}
}
