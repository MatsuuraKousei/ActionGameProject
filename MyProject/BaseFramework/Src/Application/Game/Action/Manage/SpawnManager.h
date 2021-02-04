#pragma once

#include "../../GameObject.h"

class Boar;
class Bat;
class Monkey;
class Sentry;

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

	int					m_respawnTimer = 0;

	std::string			m_sNextStage;

	std::shared_ptr<Boar> m_setBoar;
	std::shared_ptr<Bat> m_setBat;
	std::shared_ptr<Monkey> m_setMonkey;
	std::shared_ptr<Sentry> m_setSentry;
};