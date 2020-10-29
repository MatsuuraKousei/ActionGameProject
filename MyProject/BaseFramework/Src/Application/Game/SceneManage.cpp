#include "SceneManage.h"
#include "Scene.h"

void SceneManeger::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
	m_pos = Goal;
}

void SceneManeger::Update()
{
	Scene::GetInstance().AddDebugLine(m_mWorld.GetTranslation(), Math::Vector3(0.0f, 10.0f, 0.0f));

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 5;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{

		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Scene::GetInstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), info.m_radius, { 1.0f,0.0f,0.0f,1.0f }
			);

			Scene::GetInstance().RequestChangeScene("Data/JsonFile/Scene/DunGeon.json");
		}
	}
}
