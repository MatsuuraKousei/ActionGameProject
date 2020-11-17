#pragma once
#include "Scene.h"
#include"GameObject.h"

class SceneCameraManage:public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
private:
	Vector3 m_CameraTrans;
};