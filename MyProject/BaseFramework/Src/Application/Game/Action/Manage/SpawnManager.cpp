#include "SpawnManager.h"
#include "../Enemy/Boar.h"
#include "../Enemy/Alligator.h"
#include "../Enemy/Eagle.h"
#include "../Enemy/Bat.h"
#include "../Enemy/Sentry.h"
#include "../Enemy/Monkey.h"
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
		case 4:
			m_enemTag = Tag_Sentry;
			break;
		case 5:
			m_enemTag = Tag_Monkey;
			break;
		default:
			m_enemTag = Tag_None;
			break;
		}
	}

	if (jsonObj["About"].is_number())
	{
		int number = jsonObj["About"].number_value();
		switch (number)
		{
		case 0:
			m_about = Tag_OneSytem;
			break;
		case 1:
			m_about = Tag_EndlessSystem;
			break;
		default:
			m_about = Tag_OneSytem;
			break;
		}
	}

	if (jsonObj["Rot"].is_array())
	{
		auto& p = jsonObj["Rot"].array_items();
		m_rot = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}


	if (jsonObj["Radius"].is_number())
	{
		m_radius = jsonObj["Radius"].number_value();
	}
	else
	{
		m_radius = 5;
	}

	if (m_about == Tag_EndlessSystem)
	{
		auto m_spBoar = std::make_shared<Boar>();
		m_spBoar->IsEndless() = false;

		auto m_spBat = std::make_shared<Bat>();
		m_spBat->IsEndless() = false;
	}

	m_setBoar = std::make_shared<Boar>();
	m_setBat = std::make_shared<Bat>();
	m_setMonkey = std::make_shared<Monkey>();
	m_setSentry = std::make_shared<Sentry>();

	m_setBoar->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Boar.json"));
	m_setBat->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Bat.json"));
	m_setMonkey->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Monkey.json"));
	m_setSentry->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Sentry.json"));
}

void SpawnManeger::Update()
{
	Debug::GetInstance().AddDebugLine(m_mWorld.GetTranslation(), Math::Vector3(0.0f, 10.0f, 0.0f), Math::Vector4(1, 0, 0, 1));

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_radius;

	Debug::GetInstance().AddDebugSphereLine(m_mWorld.GetTranslation(), info.m_radius, { 0.0f,0.0f,1.0f,1.0f });
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			switch (m_about)
			{
			case Tag_OneSytem:
				OneSystem();
				break;
			case Tag_EndlessSystem:
				m_respawnTimer--;
				if (m_respawnTimer < 0)
				{
					m_respawnTimer = 200;
					EndlessSystem();
				}
				break;
			}
		}
		else
		{

		}
	}

	if (m_enemTag == Tag_Eagle)
	{
		//if (GetActive()) { return; }
		auto m_spEagle = std::make_shared<Eagle>();
		m_spEagle->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Eagle.json"));
		m_spEagle->m_pos.x = m_mWorld.GetTranslation().x;
		m_spEagle->m_pos.y = m_mWorld.GetTranslation().y + 10;
		m_spEagle->m_pos.z = m_mWorld.GetTranslation().z;

		m_spEagle->m_target.x = m_mWorld.GetTranslation().x;
		m_spEagle->m_target.y = m_mWorld.GetTranslation().y;
		m_spEagle->m_target.z = m_mWorld.GetTranslation().z;

		Scene::GetInstance().AddObject(m_spEagle);
		//GetActive() = true;
	}
}

void SpawnManeger::OneSystem()
{
	if (Respawn) { return; }

	auto m_spBoar = std::make_shared<Boar>();
	m_spBoar = m_setBoar;

	auto m_spAlligator = std::make_shared<Alligator>();
	m_spAlligator->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Alligator.json"));

	auto m_spBat = std::make_shared<Bat>();
	m_spBat = m_setBat;

	auto m_spSentry = std::make_shared<Sentry>();
	m_spSentry = m_setSentry;

	auto m_spMonkey = std::make_shared<Monkey>();
	m_spMonkey = m_setMonkey;


	switch (m_enemTag)
	{
	case Tag_None:
		break;
	case Tag_Boar:
		m_spBoar->m_pos.x = m_mWorld.GetTranslation().x;
		m_spBoar->m_pos.y = m_mWorld.GetTranslation().y - 2.0f;
		m_spBoar->m_pos.z = m_mWorld.GetTranslation().z;
		m_spBoar->m_rot = m_rot;
		Scene::GetInstance().AddObject(m_spBoar);
		Respawn = true;
		break;

	case Tag_Alligator:
		m_spAlligator->m_pos.x = m_mWorld.GetTranslation().x;
		m_spAlligator->m_pos.y = m_mWorld.GetTranslation().y;
		m_spAlligator->m_pos.z = m_mWorld.GetTranslation().z;
		Scene::GetInstance().AddObject(m_spAlligator);
		Respawn = true;
		break;

	case Tag_Bat:
		m_spBat->m_pos.x = m_mWorld.GetTranslation().x;
		m_spBat->m_pos.y = m_mWorld.GetTranslation().y;
		m_spBat->m_pos.z = m_mWorld.GetTranslation().z;
		m_spBat->m_rot = m_rot;
		Scene::GetInstance().AddObject(m_spBat);
		Respawn = true;
		break;

	case Tag_Sentry:
		m_spSentry->m_pos.x = m_mWorld.GetTranslation().x;
		m_spSentry->m_pos.y = m_mWorld.GetTranslation().y - 4;
		m_spSentry->m_pos.z = m_mWorld.GetTranslation().z;
		m_spSentry->m_rot = m_rot;
		Scene::GetInstance().AddObject(m_spSentry);
		Respawn = true;
		break;

	case Tag_Monkey:
		m_spMonkey->m_pos.x = m_mWorld.GetTranslation().x;
		m_spMonkey->m_pos.y = m_mWorld.GetTranslation().y - 4;
		m_spMonkey->m_pos.z = m_mWorld.GetTranslation().z;
		m_spMonkey->m_rot = m_rot;
		Scene::GetInstance().AddObject(m_spMonkey);
		Respawn = true;
		break;
	default:
		break;
	}
}

void SpawnManeger::EndlessSystem()
{
	auto m_spBoar = std::make_shared<Boar>();
	m_spBoar->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Boar.json"));

	auto m_spAlligator = std::make_shared<Alligator>();
	m_spAlligator->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Alligator.json"));

	auto m_spBat = std::make_shared<Bat>();
	m_spBat->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Bat.json"));

	switch (m_enemTag)
	{
	case Tag_None:
		break;
	case Tag_Boar:
		if (m_spBoar->IsEndless()) { return; }
		m_spBoar->m_pos.x = m_mWorld.GetTranslation().x;
		m_spBoar->m_pos.y = m_mWorld.GetTranslation().y - 2.0f;
		m_spBoar->m_pos.z = m_mWorld.GetTranslation().z;
		m_spBoar->EndlessMode = true;
		m_spBoar->IsEndless() = true;
		Scene::GetInstance().AddObject(m_spBoar);

		break;

	case Tag_Alligator:
		m_spAlligator->m_pos.x = m_mWorld.GetTranslation().x;
		m_spAlligator->m_pos.y = m_mWorld.GetTranslation().y;
		m_spAlligator->m_pos.z = m_mWorld.GetTranslation().z;
		Scene::GetInstance().AddObject(m_spAlligator);

		break;

	case Tag_Bat:
		if (m_spBat->IsEndless()) { return; }
		m_spBat->m_pos.x = m_mWorld.GetTranslation().x;
		m_spBat->m_pos.y = m_mWorld.GetTranslation().y;
		m_spBat->m_pos.z = m_mWorld.GetTranslation().z;
		m_spBat->EndlessMode = true;
		m_spBat->IsEndless() = true;
		Scene::GetInstance().AddObject(m_spBat);

		break;
	}
}
