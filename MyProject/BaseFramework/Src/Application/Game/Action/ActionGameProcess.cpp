#include "ActionGameProcess.h"
#include "../Scene.h"
#include"../SceneManage.h"
#include "Human.h"
#include"Item.h"


void ActionGameProcess::Deserialize(const json11::Json& jsonobj)
{
	// タイトルテクスチャ
	m_spTitleTex = ResFac.GetTexture("Data/Textures/2DTexture/Title/Title.png");

	m_spEnterTex = ResFac.GetTexture("Data/Textures/2DTexture/UI/Fonts/Enter.png");
	m_spSpaceTex = ResFac.GetTexture("Data/Textures/2DTexture/UI/Fonts/Space.png");

	// ゲームクリアロゴ
	m_spClearTex = ResFac.GetTexture("Data/Textures/2DTexture/GameClear/GameClearLogo.png");

	// ゲームオーバーロゴ
	m_spOverTex = ResFac.GetTexture("Data/Textures/2DTexture/GameOver/GameOverLogo.png");

	// 黒バック
	m_spBlack = ResFac.GetTexture("Data/Textures/2DTexture/UI/Back/Black.png");
	// 白バック
	m_spWhite = ResFac.GetTexture("Data/Textures/2DTexture/UI/Back/White.png");

	// UI用テクスチャの読み込み
	m_spMotherHPTex = ResFac.GetTexture("Data/Textures/2DTexture/UI/Player/HPBer.png");

	m_spDiamond = ResFac.GetTexture("Data/Textures/2DTexture/UI/Jewelry/Diamond.png");
	m_spDiaBack = ResFac.GetTexture("Data/Textures/2DTexture/UI/Back/Diamond.png");

	m_spSlash = ResFac.GetTexture("Data/Textures/2DTexture/UI/Fonts/Slash.png");

	for (int i = 0; i < 4; i++)
	{
		m_spHPTex[i] = ResFac.GetTexture("Data/Textures/2DTexture/UI/Player/HP.png");
	}

	for (int i = 0; i < 10; i++)
	{
		std::string s = "Data/Textures/2DTexture/UI/Fonts/";
		s += std::to_string(i);
		s += ".png";
		m_spNumbers[i] = ResFac.GetTexture(s);
	}

	// バトルUI

	for (int i = 0; i < 2; i++)
	{
		m_spCenterOption[i] = ResFac.GetTexture("Data/Textures/2DTexture/Battle/CenterOption.png");
	}

	m_spScope = ResFac.GetTexture("Data/Textures/2DTexture/Battle/Scope.png");

	UIPos.z = 0;
	UIPos.y += 40;
	Dia.y = 300;
	Dia.x = 500;

	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		m_WhiteOutFlg = false;
		m_WhiteOut = 0;
		break;
	case FIELD:
		m_WhiteOutFlg = true;
		m_WhiteOut = 1;
		m_BlackOutFlg = false;
		break;
	case CLEAR:
		break;
	case OVER:
		m_BlackOutFlg = true;
		m_BlackOut = 1;
		break;
	}
}

void ActionGameProcess::Draw2D()
{
	if (Scene::GetInstance().debug) { return; }
	// 2D描画
	SHADER.m_spriteShader.SetMatrix(m_mWorld);

	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		if (!m_WhiteOutFlg)
		{
			SHADER.m_spriteShader.DrawTex(m_spTitleTex.get(), 0, 200);
			SHADER.m_spriteShader.DrawTex(m_spEnterTex.get(), 0, -200);
		}
		break;
	case FIELD:
		// ダイヤ
		SHADER.m_spriteShader.DrawTex(m_spDiaBack.get(), Dia.x + 400, Dia.y);

		// 体力
		SHADER.m_spriteShader.DrawTex(m_spMotherHPTex.get(), UIPos.x, UIPos.y);
		SHADER.m_spriteShader.DrawTex(m_spDiamond.get(), Dia.x + 300, Dia.y);

		// 分母
		SHADER.m_spriteShader.DrawTex(m_spNumbers[ActionGameProcess::GetInstance().MAXDia].get(), Dia.x + 500, Dia.y);
		//「/」
		SHADER.m_spriteShader.DrawTex(m_spSlash.get(), Dia.x + 450, Dia.y);
		// 分子
		SHADER.m_spriteShader.DrawTex(m_spNumbers[ActionGameProcess::GetInstance().getDia].get(), Dia.x + 400, Dia.y);

		for (int i = 0; i < m_iPlayerHP; i++)
		{
			SHADER.m_spriteShader.DrawTex(m_spHPTex[i].get(), UIPos.x - (37 - (26 * i)), UIPos.y);
		}
		if (Human::IsSnipe())
		{
			SHADER.m_spriteShader.DrawTex(m_spScope.get(), 0, 0);
		}
		break;
	case CLEAR:
		SHADER.m_spriteShader.DrawTex(m_spClearTex.get(), 0, 0);
		SHADER.m_spriteShader.DrawTex(m_spSpaceTex.get(), 0, -200);
		break;
	case OVER:
		SHADER.m_spriteShader.DrawTex(m_spOverTex.get(), 0, m_OverY);
		SHADER.m_spriteShader.DrawTex(m_spSpaceTex.get(), 0, -200);
		break;
	}

	SHADER.m_spriteShader.DrawTex(m_spWhite.get(), 0, 0, 1, nullptr, &Math::Color(Math::Vector4(1, 1, 1, m_WhiteOut)));
	SHADER.m_spriteShader.DrawTex(m_spBlack.get(), 0, 0, 1, nullptr, &Math::Color(Math::Vector4(1, 1, 1, m_BlackOut)));
}

void ActionGameProcess::Update()
{
	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_WhiteOutFlg = true;
		}
		if (m_WhiteOutFlg)
		{
			m_WhiteOut += 0.01f;
		}
		if (m_WhiteOut > 1)
		{
			m_WhiteOut = 1;
			Scene::GetInstance().stageProcess = FIELD;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Field);
		}
		break;
	case FIELD:
		if (m_WhiteOutFlg)
		{
			m_WhiteOut -= 0.01f;
		}
		if (m_WhiteOut < 0)
		{
			m_WhiteOut = 0;
			m_WhiteOutFlg = false;
		}
		if (m_BlackOutFlg)
		{
			m_BlackOut += 0.01f;
		}
		if (m_BlackOut > 1)
		{
			m_BlackOut = 1;
			Scene::GetInstance().stageProcess = OVER;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Gameover);
		}

		for (auto pObject : Scene::GetInstance().GetObjects())
		{
			if (pObject->GetTag() & TAG_Player)
			{
				m_iPlayerHP = pObject->m_Hp;
				if (m_iPlayerHP <= 0)
				{
					m_BlackOutFlg = true;
				}
			}
		}

		if (ActionGameProcess::GetInstance().m_getFlg)
		{
			ViewDiamond();
		}
		break;
	case CLEAR:
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			Scene::GetInstance().stageProcess = OPNING;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Opning);
		}
		break;
	case OVER:
		if (m_BlackOutFlg)
		{
			m_BlackOut -= 0.01f;
		}
		if (m_BlackOut < 0)
		{
			m_BlackOut = 0;
			m_BlackOutFlg = false;

		}
		m_OverY--;
		if (m_OverY < 0)
		{
			m_OverY = 0;
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			Scene::GetInstance().stageProcess = FIELD;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Field);
		}
		break;
	}
}

void ActionGameProcess::ViewDiamond()
{
	if (!m_UIFlg)
	{
		if (Dia.x > 0)
		{
			Dia.x -= 25;
		}
		else
		{
			Dia.x = 0;
			m_UIFlg = true;
		}
	}
	else
	{
		m_UITimer--;
		if (m_UITimer < 0)
		{
			if (Dia.x < 500)
			{
				Dia.x += 25;
			}
			else
			{
				Dia.x = 500;
				m_UITimer = 30;
				m_UIFlg = false;
				ActionGameProcess::GetInstance().m_getFlg = false;
			}
		}
	}
}
