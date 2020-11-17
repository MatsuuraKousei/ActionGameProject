#include "Boss.h"
#include"../Human.h"
#include "../../Scene.h"

void Boss::Update()
{
	if (!m_alive) { return; }

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
}