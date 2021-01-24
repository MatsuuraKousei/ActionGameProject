#pragma once

#include "../../GameObject.h"

class Boss :public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;

	virtual void Update()override;

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	Vector3						m_pos;

protected:
	enum Faze
	{
		Default,
		Action,
		Attack
	};


	void Move();

	void UpdateCollision();

	void VectorMove(Matrix);

	float						m_rotateAngle = 4.0f;				//キャラクターの回転角度
	
	double						m_speed = 0.29f;

	float						m_moveAngle;

	bool						m_ActionState = false;

	Matrix						m_WonderMove;

	Faze						m_faze = Faze::Default;

	std::weak_ptr<GameObject>	m_wpTarget;

	Vector3						m_rot;
	Vector3						m_force;	// キャラの移動量
	Vector3						m_Player;
	Vector3						m_Move;
	Vector3						m_prevPos = {};

	int							AnimCounter = 2;
	int							AttackCounter = 60;
	int							WaitCounter = 15;

	bool						m_moveFlg = true;

	bool						StopMove = false;
	int i = 0;


	float						m_uniqueCol = 10.0f;
	int							m_AttackTime = 60;
};