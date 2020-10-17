#include "Enemy.h"
#include "../Scene.h"

void Enemy::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

}

void Enemy::Update()
{
	Scene::GetInstance().AddDebugLine(m_mWorld.GetTranslation(), Math::Vector3(0.0f, 10.0f, 0.0f));


	if (m_alive == false) { return; }

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
		//Wonder();
		break;
	case Action:
		Move();
		break;
	case Attack:

		break;
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
	info.m_radius = m_uniqueCol;

	if (m_faze == Default)
	{
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


				StopMove = true;

				m_wpTarget = obj;
				if (rotateRadian != 0)
				{
					m_rot.y += rotateRadian;
				}
				else
				{
					
					//m_faze = Action;

				}
			}
		}
	}

	UpdateCollision();
	PlayerSphere();
}

void Enemy::Move()
{
	if (!StopMove) { return; }

	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();

	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();


	if (target)
	{
		Matrix mat;
		mat.SetTranslation(target->GetMatrix().GetTranslation());

		VectorMove(mat);

	}
	Vector3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= (float)m_speed;

	m_force = m_force + move;
}

void Enemy::Wonder()
{
	if (!StopMove)
	{
		Vector3 vec[6];

		for (int i = 0; i < 6; i++)
		{
			Vector3 suppot;

			switch (i)
			{
			case 0:
				suppot.x = 0.0f;
				suppot.z = 5.0f;
				break;
			case 1:
				suppot.x = 2.5f;
				suppot.z = 7.5f;
				break;
			case 2:
				suppot.x = 5.0f;
				suppot.z = 5.0f;
				break;
			case 3:
				suppot.x = 5.0f;
				suppot.z = 0.0f;
				break;
			case 4:
				suppot.x = 2.5f;
				suppot.z = -2.5f;
				break;
			case 5:
				suppot.x = 0.0f;
				suppot.z = 0.0f;
				break;
			}

			vec[i] = vec[i] + suppot;
		}

		if (m_pos.x > vec[i].x)
		{
			m_force.x = -0.5f;
			if (m_pos.x == vec[i].x)
			{
				m_force.x = 0;
			}
		}
		if (m_pos.x < vec[i].x)
		{
			m_force.x = 0.5f;
			if (m_pos.x == vec[i].x)
			{
				m_force.x = 0;
			}
		}
		if (m_pos.z > vec[i].z)
		{
			m_force.z = -0.5f;
			if (m_pos.z == vec[i].z)
			{
				m_force.x = 0;
			}
		}
		if (m_pos.z < vec[i].z)
		{
			m_force.z = 0.5f;
			if (m_pos.z == vec[i].z)
			{
				m_force.x = 0;
			}
		}

		if (m_pos.x == vec[i].x && m_pos.z == vec[i].z)
		{
			i++;
		}
		if (i > 5)
		{
			i = 0;
		}
	}
}

void Enemy::UpdateCollision()
{
}

void Enemy::VectorMove(Matrix mat)
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
}

void Enemy::PlayerSphere()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_Player;
	info.m_radius = 8;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Enemy)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Scene::GetInstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), info.m_radius, { 1.0f,0.0f,0.0f,1.0f }
			);

			m_faze = Attack;
		}
	}
}
