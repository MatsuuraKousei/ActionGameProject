#pragma once

#include "../GameProcess.h"

// シューティングゲームシーンの管理と画面遷移を担当するクラス

// GameObject→GameProcess→ShootingGameProcess
class ActionGameProcess : public GameProcess
{
public:
	ActionGameProcess() {}
	virtual ~ActionGameProcess() {}

	void Update() override;
};