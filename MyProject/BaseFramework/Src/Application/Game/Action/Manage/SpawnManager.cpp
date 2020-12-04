#include "SpawnManager.h"
#include "../Enemy/Boar.h"
#include "../Enemy/Alligator.h"
#include "../Enemy/Eagle.h"
#include "../Enemy/Bat.h"
#include "../../Scene.h"
#include"../../../../System/Debug/Debug.h"

void SpawnManeger::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["EnemTag"].is_number())
	{
		int Tag = jsonObj["EnemTag"].number_value();

		switch (Tag)
		{
		case 0:
			m_enemTag = Tag_Boar;
			break;
		case 1:
			m_enemTag = Tag_Bat;
			break;
		case 2:
			m_enemTag = Tag_Alligator;
			break;
		case 3:
			m_enemTag = Tag_Eagle;
		default:
			m_enemTag = Tag_None;
			break;
		}
	}
}

void SpawnManeger::Update()
{
	Debug::GetInstance().AddDebugLine(m_mWorld.GetTranslation(), Math::Vector3(0.0f, 10.0f, 0.0f), Math::Vector4(1, 0, 0, 1));

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
			if (GetActive()) { return; }
			Debug::GetInstance().AddDebugSphereLine(m_mWorld.GetTranslation(), info.m_radius, { 0.0f,0.0f,1.0f,1.0f });
			auto m_spBoar = std::make_shared<Boar>();
			auto m_spAlligator = std::make_shared<Alligator>();
			auto m_spBat = std::make_shared<Bat>();
			

			switch (m_enemTag)
			{
			case Tag_None:
				break;
			case Tag_Boar:

				m_spBoar->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Boar.json"));

				m_spBoar->m_pos.x = m_mWorld.GetTranslation().x;
				m_spBoar->m_pos.y = m_mWorld.GetTranslation().y - 2.0f;
				m_spBoar->m_pos.z = m_mWorld.GetTranslation().z;

				Scene::GetInstance().AddObject(m_spBoar);
				GetActive() = true;
				break;

			case Tag_Alligator:

				m_spAlligator->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Alligator.json"));

				m_spAlligator->m_pos.x = m_mWorld.GetTranslation().x;
				m_spAlligator->m_pos.y = m_mWorld.GetTranslation().y;
				m_spAlligator->m_pos.z = m_mWorld.GetTranslation().z;

				Scene::GetInstance().AddObject(m_spAlligator);
				GetActive() = true;
				break;

			case Tag_Bat:
				m_spBat->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Bat.json"));

				m_spBat->m_pos.x = m_mWorld.GetTranslation().x;
				m_spBat->m_pos.y = m_mWorld.GetTranslation().y;
				m_spBat->m_pos.z = m_mWorld.GetTranslation().z;

				Scene::GetInstance().AddObject(m_spBat);
				GetActive() = true;
				break;
			default:
				break;
			}
		}
	}

	if (m_enemTag == Tag_Eagle)
	{
		if (GetActive()) { return; }
		auto m_spEagle = std::make_shared<Eagle>();
		m_spEagle->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Eagle.json"));
		m_spEagle->m_pos.x = m_mWorld.GetTranslation().x;
		m_spEagle->m_pos.y = m_mWorld.GetTranslation().y + 10;
		m_spEagle->m_pos.z = m_mWorld.GetTranslation().z;

		m_spEagle->m_target.x = m_mWorld.GetTranslation().x;
		m_spEagle->m_target.y = m_mWorld.GetTranslation().y;
		m_spEagle->m_target.z = m_mWorld.GetTranslation().z;

		Scene::GetInstance().AddObject(m_spEagle);
		GetActive() = true;
	}
}
