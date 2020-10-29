#pragma once

#include "../GameObject.h"

class Boar;

class SpawnManeger:public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	virtual void Update()override;										// 更新

	bool SpawnFlg = false;
private:

	Vector3	m_pos = {20,1,5};


	std::shared_ptr<Boar> m_spBoar = nullptr;
};