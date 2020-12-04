#pragma once

#include "../../GameObject.h"

class SpawnManeger:public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	virtual void Update()override;										// 更新

	static bool& GetActive()
	{
		static bool Active;
		return Active;
	}

private:
	enum EnemyTag
	{
		Tag_None,
		Tag_Boar,
		Tag_Alligator,
		Tag_Eagle,
		Tag_Bat
	};

	EnemyTag m_enemTag = EnemyTag::Tag_None;

	Vector3	m_pos = {20,1,5};

	std::string			m_sNextStage;
};