#include "Boss.h"
#include"../Human.h"
#include "../../Scene.h"

void Boss::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	if (jsonObj["Pos"].is_array())
	{
		auto& p = jsonObj["Pos"].array_items();
		m_pos = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	m_Hp = 50;
}

void Boss::Update()
{
	if (!m_alive) { return; }

	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 1000;
	// 行動
	switch (m_faze)
	{
	case Default:

		/*Move();
		m_rot.y += 0.009;*/
		// 球情報の作成
		for (auto& obj : Scene::GetInstance().GetObjects())
		{
			// 自分自身を無視
			if (obj.get() == this) { continue; }

			// キャラクターと当たり判定をするのでそれ以外は無視
			if (!(obj->GetTag() & TAG_Player)) { continue; }

			// 当たり判定
			if (obj->HitCheckBySphere(info))
			{

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

			}
		}
		break;
	case Action:
		break;
	case Attack:
		break;
	}
	m_prevPos = m_mWorld.GetTranslation();

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
	

	UpdateCollision();
}

void Boss::Move()
{

	Vector3 moveVec = m_mWorld.GetAxisZ();
	moveVec.Normalize();

	m_force.x = moveVec.x * 0.5f;
	m_force.z = moveVec.z * 0.5f;
}

void Boss::UpdateCollision()
{

}

void Boss::VectorMove(Matrix mat)
{

	Vector3 rMoveDir = mat.GetTranslation();

	// 単位ベクトル化：自身からターゲットへ向かうながさ１のベクトル
	rMoveDir.Normalize();

	// 自分のZ方向(前方向)
	Vector3 vZ = m_mWorld.GetAxisZ();
	// 拡大率が入っていると計算がおかしくなるため単位ベクトル化
	vZ.Normalize();

	// キャラの今向いている方向の角度を求める
	float nowRadian = atan2(vZ.x, vZ.z);

	//移動する方向へのベクトルの角度を求める（ラジアン角）
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

	// 今向いてる方向と目的の方向との差を求める
	float rotateRadian = targetRadian - nowRadian;

	// -π ～ π（-180度 ～ 180度）
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	// 一回の回転角度をm_rotateAngle以内に収める（クランプ）
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * Radians, m_rotateAngle * Radians);

	m_rot.y += rotateRadian;
	m_rot.x += rotateRadian;
}