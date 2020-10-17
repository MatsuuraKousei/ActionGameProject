#pragma once

#include "../GameObject.h"

class Item :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
private:

	// アイテム回転
	float			m_Rotate;
	// アイテム上下
	Vector3			m_Trans;
};