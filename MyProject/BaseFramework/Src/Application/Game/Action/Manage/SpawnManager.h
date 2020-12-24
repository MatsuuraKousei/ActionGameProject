#pragma once

#include "../../GameObject.h"

class SpawnManeger:public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	virtual void Update()override;										// 更新

	
	bool Respawn = false;
private:
	enum EnemyTag
	{
		Tag_None,
		Tag_Boar,
		Tag_Alligator,
		Tag_Eagle,
		Tag_Bat,
		Tag_Sentry,
		Tag_Monkey
	}m_enemTag = Tag_None;

	enum About
	{
		Tag_OneSytem,
		Tag_EndlessSystem
	}m_about = Tag_OneSytem;

	void OneSystem();
	void EndlessSystem();

	Vector3				m_pos;
	Vector3				m_rot;

	float				m_radius;

	int					m_respawnTimer = 30;

	std::string			m_sNextStage;
};