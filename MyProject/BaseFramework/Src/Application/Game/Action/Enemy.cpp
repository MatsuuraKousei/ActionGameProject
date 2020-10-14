#include "Enemy.h"
#include "../Scene.h"

void Enemy::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

}

void Enemy::Update()
{
	m_prevPos = m_mWorld.GetTranslation();

	for (auto spOblect : Scene::GetInstance().GetObjects())
	{
		if (spOblect->GetTag() == TAG_Player)
		{
			m_Player = spOblect->GetMatrix().GetTranslation();
		}
	}

	// 行動
	switch (m_faze)
	{
	case Default:
		m_XFlg = false;
		m_ZFlg = false;
		break;
	case Action:
		Move();
		break;
	case Attack:
		m_AttackTime--;
		if (m_AttackTime < 0)
		{
			m_faze = Default;
			m_AttackTime = 60;
		}
		break;
	default:
		break;
	}

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_uniqueCol;

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
				m_mWorld.GetTranslation(), m_uniqueCol, { 1.0f,0.0f,0.0f,1.0f }
			);

			m_faze = Action;
		}
	}
	
	m_mWorld.Move(m_Move);
}

void Enemy::Move()
{
	if (m_mWorld.GetTranslation().x <= m_Player.x + 2)
	{
		m_Move.x = 0.2f;
		if (m_mWorld.GetTranslation().x > m_Player.x + 1)
		{
			m_Move.x = 0;
			m_XFlg = true;
		}
	}
	if (m_mWorld.GetTranslation().x >= m_Player.x - 2)
	{
		m_Move.x = -0.2f;
		if (m_mWorld.GetTranslation().x < m_Player.x - 1)
		{
			m_Move.x = 0;
			m_XFlg = true;
		}
	}
	if (m_mWorld.GetTranslation().z <= m_Player.z + 2)
	{
		m_Move.z = 0.2f;
		if (m_mWorld.GetTranslation().z > m_Player.z + 1)
		{
			m_Move.z = 0;
			m_ZFlg = true;
		}
	}
	if (m_mWorld.GetTranslation().z >= m_Player.z - 2)
	{
		m_Move.z = -0.2f;
		if (m_mWorld.GetTranslation().z < m_Player.z - 1)
		{
			m_Move.z = 0;
			m_ZFlg = true;
		}
	}

	if (m_XFlg && m_ZFlg)
	{
		m_faze = Attack;
	}
}
