#include "ScoreManager.h"

void ScoreManager::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);
}

void ScoreManager::Update()
{
	if (GetInstance().Calculation)
	{
		// ダイヤモンドによるスコア換算
		m_iAllDiamondsScore += GetInstance().AllDiamonds * 100;

		// 敵撃破によるスコア換算
		m_iAllEnemiesScore += GetInstance().AllEnemies * 10;


		// 総合得点
		m_iTotalScore = m_iAllDiamondsScore + m_iAllEnemiesScore;
		
		// 引き渡し
		GetInstance().TotalScore = m_iTotalScore;
	}
}
