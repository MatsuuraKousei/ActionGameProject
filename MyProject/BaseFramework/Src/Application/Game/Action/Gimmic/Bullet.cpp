#include "Bullet.h"
#include "../../../main.h"

void Bullet::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// 生きる時間(最大Fpsに依存?)
	m_lifeSpan = APP.m_maxFps * 10;
}

void Bullet::Update()
{
	// 死んでいる場合早期リターン
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		// 消すかどうかはSceneが見る
		Destroy();
	}

	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();

	Vector3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= (float)m_speed;

	m_force = m_force + move;

	m_rot.y = m_GimmicAngle;

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Bullet::GetGimmicAngle(float f)
{
	m_GimmicAngle = f;
}