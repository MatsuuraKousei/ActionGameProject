#pragma once

#include "../GameObject.h"

enum Faze
{
	Default,
	Action,
	Attack
};

class Enemy :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
private:
	void Move();

	Faze						m_faze = Faze::Default;

	Vector3						m_Player;
	Vector3						m_Move;
	Vector3						m_prevPos = {};

	float						m_uniqueCol = 10.0f;

	bool						m_XFlg = false;
	bool						m_ZFlg = false;

	int							m_AttackTime = 60;
};