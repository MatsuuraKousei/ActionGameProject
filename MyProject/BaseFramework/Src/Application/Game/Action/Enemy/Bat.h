#pragma once

#include "../../GameObject.h"


class Bat :public GameObject
{
public:

	Bat();

	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	Vector3						m_pos;
	Vector3						m_rot;
	Vector3						m_target;

	bool						EndlessMode = false;
	static bool& IsEndless()
	{
		static bool endless;
		return endless;
	}

private:

	enum Faze
	{
		Default,
		Action,
		Attack
	};

	void Appearance();

	void Move();

	void HP();

	void Shot();

	void Explosion(const Vector3& hitPos);
	
	float						m_moveAngle;

	float						m_rotateAngle = 4.0f;				//キャラクターの回転角度

	double						m_speed = 0.29f;

	bool						m_ActionState = false;

	bool						m_Animation = false;

	bool						m_WingFlg[2] = { false,false };

	int							m_AttackState = 80;

	float roop = 9;

	bool f=true;

	Matrix						m_WonderMove;

	Faze						m_faze = Faze::Default;

	std::weak_ptr<GameObject>	m_wpTarget;


	Vector3						m_force;	// キャラの移動量

	Vector3						m_Player;
	Vector3						m_Move;
	Vector3						m_prevPos = {};

	int							AnimCounter = 2;
	int							AttackCounter = 60;
	int							WaitCounter = 15;

	bool						StopMove = false;
	int i = 0;

	bool						m_moveFlg = true;


	float						m_uniqueCol = 15.0f;
	int							m_AttackTime = 60;
};