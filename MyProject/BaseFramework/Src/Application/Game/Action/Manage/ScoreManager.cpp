#include "ScoreManager.h"
#include "../../Scene.h"

void ScoreManager::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
}

void ScoreManager::Update()
{
	if (GetInstance().Calculation)
	{
		// ダイヤモンドによるスコア換算
		m_iAllDiamondsScore = GetInstance().AllDiamonds * 1000;

		// 敵撃破によるスコア換算
		m_iAllEnemiesScore = GetInstance().AllEnemies * 100;


		// 総合得点
		m_iTotalScore = m_iAllDiamondsScore + m_iAllEnemiesScore;
		
		// 引き渡し
		GetInstance().TotalScore = m_iTotalScore;

		Scene::GetInstance().ManageScore = m_iTotalScore;
	}
}
