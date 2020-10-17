#include "ActionGameProcess.h"
#include "../Scene.h"


void ActionGameProcess::Deserialize(const json11::Json& jsonobj)
{
	// UI用テクスチャの読み込み
	m_spMotherHPTex = ResFac.GetTexture("Data/Textures/2DTexture/UI/Player/HPBer.png");

	for (int i = 0; i < 4; i++)
	{
		m_spHPTex[i] = ResFac.GetTexture("Data/Textures/2DTexture/UI/Player/HP.png");
	}

	UIPos.z = 0;
	UIPos.y += 40;

}

void ActionGameProcess::Draw2D()
{

	for (auto pObject : Scene::GetInstance().GetObjects())
	{
		if (pObject->GetTag() & TAG_Player)
		{
			m_iPlayerHP= pObject->m_Hp;
		}
	}

	// 2D描画
	SHADER.m_spriteShader.SetMatrix(m_mWorld);
	SHADER.m_spriteShader.DrawTex(m_spMotherHPTex.get(), UIPos.x, UIPos.y);

	for (int i = 0; i < m_iPlayerHP; i++)
	{
		SHADER.m_spriteShader.DrawTex(m_spHPTex[i].get(), UIPos.x - (37 - (26 * i)), UIPos.y);
	}
}

void ActionGameProcess::Update()
{
	

}
