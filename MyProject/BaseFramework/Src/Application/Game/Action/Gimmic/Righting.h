#pragma once

#include "../../GameObject.h"

class Righting :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新

private:

	float		m_RightCol;

	Vector3		m_color = { 0.1f,0.1f,0.1f };
};