#pragma once

#include "GameObject.h"

//ゲームシーンの管理者をカテゴリ分けするために、GameProcessにGameObjectを継承させる
class GameProcess :public GameObject
{
public:
	GameProcess(){}
	virtual ~GameProcess() {}
protected:
	bool m_sceneChanger = false;
};