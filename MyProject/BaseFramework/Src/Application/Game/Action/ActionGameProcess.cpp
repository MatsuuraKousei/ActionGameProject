#include "ActionGameProcess.h"
#include "../Scene.h"
#include"../SceneManage.h"
#include "Human.h"
#include"Item.h"


void ActionGameProcess::Deserialize(const json11::Json& jsonobj)
{
	// タイトルテクスチャ
	m_spTitleTex = ResFac.GetTexture("Data/Textures/2DTexture/Title/Title.png");

	m_spBlack = ResFac.GetTexture("Data/Textures/2DTexture/UI/Back/Black.png");
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
}

void ActionGameProcess::Draw2D()
{
	if (Scene::GetInstance().debug) { return; }
	// 2D描画
	SHADER.m_spriteShader.SetMatrix(m_mWorld);

	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		SHADER.m_spriteShader.DrawTex(m_spTitleTex.get(), 0, 200);
		SHADER.m_spriteShader.DrawTex(m_spWhite.get(), 0, 0, nullptr, &Math::Color(Math::Vector4(1, 1, 1, 1)));
		break;
	case FIELD:
		SHADER.m_spriteShader.DrawTex(m_spDiaBack.get(), Dia.x + 400, Dia.y);
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
		break;
	case OVER:
		break;
	}
}

void ActionGameProcess::Update()
{
	switch (Scene::GetInstance().stageProcess)
	{
	case OPNING:
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Field);
			Scene::GetInstance().stageProcess = FIELD;
		}
		break;
	case FIELD:
		for (auto pObject : Scene::GetInstance().GetObjects())
		{
			if (pObject->GetTag() & TAG_Player)
			{
				m_iPlayerHP = pObject->m_Hp;
				if (m_iPlayerHP <= 0)
				{
					Scene::GetInstance().stageProcess = OVER;
					Scene::GetInstance().RequestChangeScene(Scene::GetInstance().Gameover);
				}
			}
		}
		if (ActionGameProcess::GetInstance().m_getFlg)
		{
			ViewDiamond();
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
