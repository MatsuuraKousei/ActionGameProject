﻿#pragma once

#include "../../GameObject.h"

class Boar :public GameObject
{
public:
	Boar();

	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	Vector3						m_pos;
	Vector3						m_rot;

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


	void Move();

	void Snipe();

	void UpdateCollision();

	void VectorMove(Matrix);

	void Hp();

	void Explosion(const Vector3& hitPos);

	float						m_rotateAngle = 4.0f;				//キャラクターの回転角度
	
	double						m_speed = 0.29f;

	bool						m_ActionState = false;

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
	bool						m_falseFlg=false;
	int i = 0;


	float						m_uniqueCol = 10.0f;
	int							m_AttackTime = 60;
};