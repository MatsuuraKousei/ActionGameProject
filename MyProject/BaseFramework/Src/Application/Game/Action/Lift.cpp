#include "Lift.h"

void Lift::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//最初の地点を覚える
	m_mStart = m_mWorld;

	//移動先情報の読み込み
	if (jsonObj["MoveTo"].is_array())
	{
		auto& p = jsonObj["MoveTo"].array_items();
		m_vGoal = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	//移動スピードの読み込み
	if (jsonObj["Speed"].is_number())
	{
		m_speed = jsonObj["Speed"].number_value();
	}

	//回転の読み込み
	if (jsonObj["RotSpeed"].is_array())
	{
		auto& p = jsonObj["RotSpeed"].array_items();
		m_vRotSpeed = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}
	m_vRot.x = m_vRotSpeed.x;
	m_vRot.y = m_vRotSpeed.y;
	m_vRot.z = m_vRotSpeed.z;
}

void Lift::Update()
{
	//動く前の行列を覚える
	GameObject::Update();

	//スタート地点とゴール地点の座標を取得
	auto& vStart = m_mStart.GetTranslation();
	auto& vGoal = m_vGoal;

	

	//回転
	if (m_vRot.x >= 360)
	{
		m_vRot.x = 0;
	}
	if (m_vRot.y >= 360)
	{
		m_vRot.y = 0;
	}
	if (m_vRot.z >= 360)
	{
		m_vRot.z = 0;
	}
	
	m_mWorld.RotateX(m_vRot.x);
	m_mWorld.RotateY(m_vRot.y);
	m_mWorld.RotateZ(m_vRot.z);

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
