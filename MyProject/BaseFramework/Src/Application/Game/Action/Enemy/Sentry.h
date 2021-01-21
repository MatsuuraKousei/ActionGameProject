#pragma once

#include "../../GameObject.h"

class Bullet;

class Sentry :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	Vector3						ResPos;
	Vector3						m_pos;
	Vector3						m_rot;

private:
	enum Faze
	{
		Default,
		Action,
		Attack
	}m_faze = Default;

	void Snipe();

	void Shot();

	void Explosion();

	std::weak_ptr<GameObject>	m_wpTarget;

	std::shared_ptr<Bullet> m_spBullet;

	Model::Node* Head;

	int							m_FireLate = 60;

	float						m_uniqueCol = 30.0f;
	float						m_rotateAngle = 0.5f;				//キャラクターの回転角度

	Vector3						m_force;							// キャラの移動量
	Vector3						m_prevPos = {};
};