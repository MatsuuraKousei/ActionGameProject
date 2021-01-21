#pragma once

#include "../../GameObject.h"

class ScoreManager :public GameObject
{
public:
	// シングルトン
	static ScoreManager& GetInstance()
	{
		static ScoreManager instance;
		return instance;
	}

	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	virtual void Update()override;										// 更新

	//////受け取り受け渡し用変数群(シングルトン管理)////////////////////////////////////////
	int		AllDiamonds;												// ダイヤモンド数
	int		AllEnemies;													// 敵撃破数
	bool	Calculation = false;										// 計算フラグ

	int		TotalScore;													// トータルスコア
	////////////////////////////////////////////////////////////////////////////////////////
private:

	int		m_iAllDiamondsScore;										// ダイヤモンド習得スコア
	int		m_iAllEnemiesScore;											// 敵撃破スコア

	int		m_iTotalScore;												// トータルスコア

};