#include "Boar.h"
#include"../Human.h"
#include "../../Scene.h"
#include"../Manage/SpawnManager.h"

void Boar::Update()
{
	if (!m_alive) { return; }

	m_prevPos = m_mWorld.GetTranslation();

	// 行動
	switch (m_faze)
	{
	case Default:
		m_force.y = 0.1f;
		if (m_pos.y > 1)
		{
			m_pos.y = 1;
			m_faze = Action;
		}
		break;
	case Action:
		Snipe();
		break;
	case Attack:
		Move();
		break;
	}

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);



	UpdateCollision();
}

void Boar::Move()
{
	if (!StopMove) { return; }

	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();

	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();


	
	if (AttackCounter > 0)
	{
		Vector3 move = m_mWorld.GetAxisZ();
		move.Normalize();

		move *= (float)m_speed;

		m_force = m_force + move;

		AttackCounter--;

	}
	else
	{
		StopMove = true;
		if (WaitCounter < 0)
		{
			// 球情報の作成
			SphereInfo info;
			info.m_pos = m_mWorld.GetTranslation();
			info.m_radius = m_uniqueCol-5;
			for (auto& obj : Scene::GetInstance().GetObjects())
			{
				// 自分自身を無視
				if (obj.get() == this) { continue; }

				// キャラクターと当たり判定をするのでそれ以外は無視
				if (!(obj->GetTag() & TAG_Player)) { continue; }

				// 当たり判定
				if (obj->HitCheckBySphere(info))
				{
					if (!m_falseFlg)
					{
						m_faze = Action;
					}
				}
				else
				{
					m_falseFlg = true;
				}
			}
			if (m_falseFlg)
			{
				m_force.y = -0.1f;
				if (m_pos.y < -2)
				{
					m_alive = false;
					SpawnManeger::GetActive() = false;
				}
			}
		}
		WaitCounter--;
	}
}

void Boar::Snipe()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_uniqueCol+5;
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


			StopMove = true;

			if (rotateRadian != 0)
			{
				m_rot.y += rotateRadian;
			}

			if (rotateRadian<0.02 && rotateRadian>-0.02)
			{
				AttackCounter = 60;
				m_wpTarget = obj;
				m_faze = Attack;
			}
		}

	}
}

void Boar::UpdateCollision()
{
	
}

void Boar::VectorMove(Matrix mat)
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