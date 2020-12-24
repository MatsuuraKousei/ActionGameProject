#include "Monkey.h"
#include "../../Scene.h"

void Monkey::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	

	m_Hp = 3;

	m_spShield = std::make_shared<GameObject>();
	m_spShield->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Shield.json"));
	m_spShield->m_Hp = 50;
	m_spShield->m_colRadius = 0.5f;
	Scene::GetInstance().AddObject(m_spShield);

}

void Monkey::Update()
{
	
	if (!m_alive)
	{
		m_spShield->Destroy();
	}

	if (!m_alive) { return; }

	Matrix mat;
	mat.RotateY(m_rot.y+4.7);
	mat.SetTranslation(m_mWorld.GetAxisZ() + m_mWorld.GetTranslation()+Vector3(0,1,0));
	m_spShield->SetMatrix(mat);

	switch (m_faze)
	{
	case Default:
		m_force.y = 0.1f;
		if (m_pos.y > 4.5f)
		{
			m_pos.y = 4.5f;
			m_faze = Action;
		}
		break;
	case Action:
		Move();
		break;
	case Attack:
		break;
	}

	m_pos += m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Monkey::Move()
{
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
			/*Scene::GetInstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), m_uniqueCol, { 1.0f,0.0f,0.0f,1.0f }
			);*/

			Vector3 m_Player;

			m_Player = obj->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();

			//今のキャラクターの方向ベクトル
			Vector3 nowDir = m_mWorld.GetAxisZ();
			nowDir.Normalize();

			//キャラクターの今向いている方向の角度を求める（ラジアン角）
			float nowRadian = atan2(nowDir.x, nowDir.z);

			//移動方向へのベクトル方向の角度を求める（ラジアン角）
			float targetRadian = atan2(m_Player.x, m_Player.z);

			float rotateRadian = targetRadian - nowRadian;

			//atan2の結果-π～π（-180～180度）
			//180度の角度で数値の切れ目がある
			if (rotateRadian > M_PI)
			{
				rotateRadian -= 2 * float(M_PI);
			}
			if (rotateRadian < -M_PI)
			{
				rotateRadian += 2 * float(M_PI);
			}

			//一回のの回転角度をm_rotateAngle
			rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * Radians, m_rotateAngle * Radians);

			if (rotateRadian != 0)
			{
				m_rot.y += rotateRadian;
			}

			if (rotateRadian<0.02 && rotateRadian>-0.02)
			{
				Vector3 move = m_mWorld.GetAxisZ();
				move.Normalize();

				move *= (float)m_speed;

				m_force = m_force + move;
			}
		}

	}
}
