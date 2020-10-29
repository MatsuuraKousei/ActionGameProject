#pragma once
#include "Scene.h"
#include "GameObject.h"

class SceneManeger:public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	Vector3 Goal = { -70,1,-19.2 };
	Vector3	m_pos;
	Vector3	m_rot;
};