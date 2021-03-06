﻿#pragma once

#include "../GameProcess.h"
#include "../../Component/AudioComponent.h"
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

	int						m_OverY = 500;

	DirectX::SimpleMath::Rectangle rc;
	std::shared_ptr<Texture> m_spTitleTex = nullptr;;			// タイトル
	std::shared_ptr<Texture> m_spEnterTex = nullptr;			// タイトル
	std::shared_ptr<Texture> m_spSpaceTex = nullptr;			// タイトル
	std::shared_ptr<Texture> m_spClearTex = nullptr;			// クリア
	std::shared_ptr<Texture> m_spOverTex = nullptr;			// オーバー
	std::shared_ptr<Texture> m_spMotherHPTex = nullptr;		// HPバー
	std::shared_ptr<Texture> m_spWolfHPTex = nullptr;			// WolfHPバー
	std::shared_ptr<Texture> m_spBMotherHPTex = nullptr;		// BossMotherHPバー
	std::shared_ptr<Texture> m_spDiamond = nullptr;			// ダイアモンドUI
	std::shared_ptr<Texture> m_spDiaBack = nullptr;			// ダイアモンドUI
	std::shared_ptr<Texture> m_spHPTex[4];			// HPゲージ
	std::shared_ptr<Texture> m_spNumbers[10];		// 数字
	std::shared_ptr<Texture> m_spScores[10];		// スコア
	std::shared_ptr<Texture> m_spScoreBoard = nullptr;		// スコアボード
	std::shared_ptr<Texture> m_spSlash = nullptr;				// 「/」

	std::shared_ptr<Texture> m_spBlack;				// 黒
	std::shared_ptr<Texture> m_spWhite;				// 白

	std::shared_ptr<Texture> m_spCenterOption[2];	// ロックオンRotate
	std::shared_ptr<Texture> m_spScope;				// スコープ

	int						m_UITimer = 30;
	bool					m_UIFlg = false;

	float					m_WhiteOut = 0.0f;
	bool					m_WhiteOutFlg = false;
	float					m_BlackOut = 0.0f;
	bool					m_BlackOutFlg = false;;

	int						m_Score = 00000;				// スコア

	// Audioes//////////////////////////////////////////////////////////

	std::shared_ptr<SoundInstance> m_spSystemSE;
};