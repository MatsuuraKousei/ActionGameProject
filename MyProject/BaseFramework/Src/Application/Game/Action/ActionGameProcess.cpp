#include "ActionGameProcess.h"
#include "../Scene.h"
#include "Manage/SceneManage.h"
#include "Manage/ScoreManager.h"
#include "Human.h"
#include "Item.h"


void ActionGameProcess::Deserialize(const json11::Json& jsonobj)
{
	// タイトルテクスチャ
	m_spTitleTex = ResFac.GetTexture("Data/Textures/2DTexture/Title/Title.png");
	m_spWolfHPTex = ResFac.GetTexture("Data/Textures/2DTexture/Enemy/BossHP/WolfLife.png");
	m_spBMotherHPTex = ResFac.GetTexture("Data/Textures/2DTexture/Enemy/BossHP/HPMother.png");
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

	// スコアテキスト
	m_spScoreBoard = ResFac.GetTexture("Data/Textures/2DTexture/GameClear/TotalScoresLogo.png");

	m_spDiamond = ResFac.GetTexture("Data/Textures/2DTexture/UI/Jewelry/Diamond.png");
	m_spDiaBack = ResFac.GetTexture("Data/Textures/2DTexture/UI/Back/Diamond.png");

	m_spSlash = ResFac.GetTexture("Data/Textures/2DTexture/UI/Fonts/Slash.png");

	for (int i = 0; i < 4; i++)
	{
		m_spHPTex[i] = ResFac.GetTexture("Data/Textures/2DTexture/UI/Player/HP.png");
	}

	// 数字画像情報取得
	for (int i = 0; i < 10; i++)
	{
		std::string s = "Data/Textures/2DTexture/UI/Fonts/";
		s += std::to_string(i);
		s += ".png";
		m_spNumbers[i] = ResFac.GetTexture(s);
	}

	// 上に同じく（画像違う)
	for (int i = 0; i < 10; i++)
	{
		std::string s = "Data/Textures/2DTexture/UI/ScoreFonts/";
		s += std::to_string(i);
		s += ".png";
		m_spScores[i] = ResFac.GetTexture(s);
	}

	// バトルUI

	for (int i = 0; i < 2; i++)
	{
		m_spCenterOption[i] = ResFac.GetTexture("Data/Textures/2DTexture/Battle/CenterOption.png");
	}

	m_spScope = ResFac.GetTexture("Data/Textures/2DTexture/Battle/Scope.png");

	UIPos.x = -500;
	UIPos.y = 300;
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

	m_iPlayerHP = Human::GetInstance().m_Hp;

	// Audio初期化///////////////////////////////////////////////
	m_spSystemSE = m_spSystemSE->Deserialize(Track::System);
}

#define HPMainX 80
#define HPhosei 45
#define ScoreCounterStop 99999

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
			if (m_spTitleTex.get())
			{
				SHADER.m_spriteShader.DrawTex(m_spTitleTex.get(), 0, 200, 1.1);
			}
			if (m_spEnterTex.get())
			{
				SHADER.m_spriteShader.DrawTex(m_spEnterTex.get(), 0, -200);
			}
			//SHADER.m_spriteShader.DrawTex(m_spBMotherHPTex.get(), -45, -300);
			//SHADER.m_spriteShader.DrawTex(m_spWolfHPTex.get(), 0, -300);
		}
		break;
	case FIELD:
	
		if (m_spDiaBack.get())
		{
			// ダイヤ
			SHADER.m_spriteShader.DrawTex(m_spDiaBack.get(), Dia.x + 400, Dia.y);
		}

		if (m_spMotherHPTex.get())
		{
			// 体力
			SHADER.m_spriteShader.DrawTex(m_spMotherHPTex.get(), UIPos.x, UIPos.y);
		}
		for (int i = 0; i < m_iPlayerHP; i++)
		{
			if (m_spHPTex[i].get())
			{
				SHADER.m_spriteShader.DrawTex(m_spHPTex[i].get(), UIPos.x - (HPMainX - (HPhosei * i)), UIPos.y);
			}
		}

		if (m_spDiamond.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spDiamond.get(), Dia.x + 300, Dia.y);
		}

		if (m_spNumbers[GetInstance().MAXDia].get())
		{
			// 分母
			SHADER.m_spriteShader.DrawTex(m_spNumbers[GetInstance().MAXDia].get(), Dia.x + 500.0f, Dia.y);
		}
		if (m_spSlash.get())
		{
			//「/」
			SHADER.m_spriteShader.DrawTex(m_spSlash.get(), Dia.x + 450, Dia.y);
		}
		if (m_spNumbers[GetInstance().getDia].get())
		{
			// 分子
			SHADER.m_spriteShader.DrawTex(m_spNumbers[GetInstance().getDia].get(), Dia.x + 400.0f, Dia.y);
		}


		if (Human::IsSnipe())
		{
			if (m_spScope.get())
			{
				SHADER.m_spriteShader.DrawTex(m_spScope.get(), 0, -30);
			}
		}


		break;
	case CLEAR:
		if (m_spClearTex.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spClearTex.get(), 0, 0);
		}
		if (m_spSpaceTex.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spSpaceTex.get(), 0, -200);
		}
		if (m_spScoreBoard.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spScoreBoard.get(), -150, -100);
		}
		for (int i = 0; i < 5; i++)
		{
			int bet = 0;
			int score = m_Score;
			static int Paragraph;

			if (score > ScoreCounterStop)
			{
				score = ScoreCounterStop;
			}

			switch (i)
			{
			case 0:
				bet = score / 10000;
				Paragraph = bet * 10000;
				break;
			case 1:
				score -= Paragraph;
				bet = score / 1000;
				Paragraph += bet * 1000;
				break;
			case 2:
				score -= Paragraph;
				bet = score / 100;
				Paragraph += bet * 100;
				break;
			case 3:
				score -= Paragraph;
				bet = score / 10;
				Paragraph += bet * 10;
				break;
			case 4:
				score -= Paragraph;
				bet = score;
				break;
			}
			if (m_spScores[bet].get())
			{
				SHADER.m_spriteShader.DrawTex(m_spScores[bet].get(), 100 + (i * 35), -110);
			}
		}
		break;
	case OVER:
		if (m_spOverTex.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spOverTex.get(), 0, m_OverY);
		}
		if (m_spSpaceTex.get())
		{
			SHADER.m_spriteShader.DrawTex(m_spSpaceTex.get(), 0, -200);
		}
		break;
	}

	if (m_spWhite.get())
	{
		SHADER.m_spriteShader.DrawTex(m_spWhite.get(), 0, 0, 1, nullptr, &Math::Color(Math::Vector4(1, 1, 1, m_WhiteOut)));
	}
	if (m_spBlack.get())
	{
		SHADER.m_spriteShader.DrawTex(m_spBlack.get(), 0, 0, 1, nullptr, &Math::Color(Math::Vector4(1, 1, 1, m_BlackOut)));
	}
}

void ActionGameProcess::Update()
{
	Vector3 pos = { 0,10,0 };
	
	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			m_spSystemSE->Play();
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
			m_WhiteOut -= 0.02f;
			SHADER.ResetPointLight();
		}
		if (m_WhiteOut < 0)
		{
			m_WhiteOut = 0;
			m_WhiteOutFlg = false;
		}
		if (m_BlackOutFlg)
		{
			m_BlackOut += 0.05f;
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



		if (GetInstance().m_getFlg)
		{
			ViewDiamond();
		}
		break;
	case CLEAR:
		ScoreManager::GetInstance().Calculation = true;
		m_Score = Scene::GetInstance().ManageScore;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_spSystemSE->Play();
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
			m_spSystemSE->Play();
			Scene::GetInstance().stageProcess = FIELD;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Field);
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			return;
			Scene::GetInstance().stageProcess = FIELD;
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Field2);
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
				GetInstance().m_getFlg = false;
			}
		}
	}
}
