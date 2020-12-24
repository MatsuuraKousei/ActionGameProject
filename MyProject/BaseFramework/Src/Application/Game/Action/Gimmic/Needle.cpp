#include "Needle.h"
#include "../../Scene.h"

void Needle::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_mStart = m_mWorld;

	m_vGoal = Vector3(0, 2.5, 0) + m_mWorld.GetTranslation();

	m_speed = 0.01f;
}

void Needle::Update()
{
	//スタート地点とゴール地点の座標を取得
	auto& vStart = m_mStart.GetTranslation();
	auto& vGoal = m_vGoal;

	//目標地点までのベクトル
	Vector3 vTo = vGoal - vStart;
	//進行具合を加味して現在の地点を割り出す
	Vector3 vNow = vStart + vTo * m_progress;
	//地点を中間に合わせる
	m_mWorld.SetTranslation(vNow);

	//進行具合の更新
	if (m_goTo)
	{
		//向かっている時
		m_progress += m_speed;
		if (m_progress >= 1.0f)
		{
			m_goTo = false;
			m_progress = 1.0f;
		}
	}
	else
	{
		//引き返している時
		m_progress -= m_speed;
		if (m_progress <= 0.0f)
		{
			m_goTo = true;
			m_progress = 0.0f;
		}
	}
}