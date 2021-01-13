#include "Item.h"
#include"../Scene.h"
#include"ActionGameProcess.h"
#include "Manage/ScoreManager.h"

void Item::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);


	if (jsonObj["Pos"].is_array())
	{
		auto& p = jsonObj["Pos"].array_items();
		m_pos = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	m_Rotate = 0.0f;
	m_Trans = { 0,0,0 };

	ActionGameProcess::GetInstance().MAXDia++;
}

void Item::Update()
{
	if (!m_moveFlg)
	{
		m_force.y += 0.01f;
		m_Max++;
		if (m_Max > 5)
		{
			m_moveFlg = true;
		}
	}
	else
	{
		m_force.y -= 0.01f;
		m_Max--;
		if (m_Max < 0)
		{
			m_moveFlg = false;
		}
	}

	m_rot.y ++;
	if (m_rot.y > 360)
	{
		m_rot.y = 0;
	}

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);


	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 0;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{

		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			ActionGameProcess::GetInstance().getDia++;
			ActionGameProcess::GetInstance().m_getFlg = true;
			ScoreManager::GetInstance().AllDiamonds++;
			m_alive = false;
		}
	}
}