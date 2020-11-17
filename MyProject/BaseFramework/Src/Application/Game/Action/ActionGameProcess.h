#pragma once

#include "../GameProcess.h"
#include "Application/Game/Scene.h"

class ActionGameProcess : public GameProcess
{
public:
	// シングルトン
	static ActionGameProcess& GetInstance()
	{
		static ActionGameProcess instance;
		return instance;
	}


	ActionGameProcess() {}
	virtual ~ActionGameProcess() {}

	void Deserialize(const json11::Json& jsonobj) override;
	void Draw2D();

	void Update() override;

	void ViewDiamond();

	int						getDia;
	int						MAXDia;

	Math::Vector2			Dia;
	bool					m_getFlg = false;;

private:
	Vector3					UIPos;

	int						m_iPlayerHP;

	std::shared_ptr<Texture> m_spTitleTex;		// タイトル
	std::shared_ptr<Texture> m_spMotherHPTex;		// HPバー
	std::shared_ptr<Texture> m_spDiamond;			// ダイアモンドUI
	std::shared_ptr<Texture> m_spDiaBack;			// ダイアモンドUI
	std::shared_ptr<Texture> m_spHPTex[4];			// HPゲージ
	std::shared_ptr<Texture> m_spNumbers[10];		// 数字
	std::shared_ptr<Texture> m_spSlash;				// 「/」

	int						m_UITimer = 30;
	bool					m_UIFlg=false;
};