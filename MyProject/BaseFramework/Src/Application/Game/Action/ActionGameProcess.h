#pragma once

#include "../GameProcess.h"
#include "Application/Game/Scene.h"

class ActionGameProcess : public GameProcess
{
public:
	ActionGameProcess() {}
	virtual ~ActionGameProcess() {}

	void Deserialize(const json11::Json& jsonobj) override;
	void Draw2D();

	void Update() override;

private:
	Vector3					UIPos;
	int						m_iPlayerHP;
	std::shared_ptr<Texture> m_spMotherHPTex;
	std::shared_ptr<Texture> m_spHPTex[4];
};