#pragma once

#include "../GameObject.h"

enum Faze
{
	Default,
	Targetting,
	Action,
	Attack
};

class Enemy :public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

private:
	void Move();

	void Wonder();

	//void TargettingObj();

	void UpdateCollision();

	void VectorMove(Matrix);

	void PlayerSphere();

	float						m_rotateAngle = 4.0f;				//キャラクターの回転角度
	Vector3 m_rot;
	Vector3 m_pos;
	Vector3 m_force;	// キャラの移動量

	double						m_speed = 0.5f;

	bool						m_ActionState = false;

	Matrix						m_WonderMove;

	Faze						m_faze = Faze::Default;

	std::weak_ptr<GameObject>	m_wpTarget;

	Vector3						m_Player;
	Vector3						m_Move;
	Vector3						m_prevPos = {};

	bool						StopMove = false;
	int i = 0;


	float						m_uniqueCol = 20.0f;

	bool						m_XFlg = false;
	bool						m_ZFlg = false;

	int							m_AttackTime = 60;
};