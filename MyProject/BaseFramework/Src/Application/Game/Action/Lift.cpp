#include "Lift.h"

void Lift::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	//最初の地点を覚える
	m_mStart = m_mWorld;

	if (jsonObj["LiftTag"].is_number())
	{
		int Tag = jsonObj["LiftTag"].number_value();
		switch (Tag)
		{
		case 0:
			lifttag = Normal;
			break;
		case 1:
			lifttag = Point;
			break;
		default:
			break;
		}
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

	//移動スピードの読み込み
	if (jsonObj["Speed"].is_number())
	{
		m_speed = jsonObj["Speed"].number_value();
	}

	switch (lifttag)
	{
	case Lift::Normal:
		NormalLift(jsonObj);
		break;
	case Lift::Point:
		StageLift(jsonObj);
		break;
	}

}

void Lift::Update()
{
	//動く前の行列を覚える
	GameObject::Update();

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

	switch (lifttag)
	{
	case Lift::Normal:
		SToG();
		break;
	case Lift::Point:
		PointStage();
		break;
	}
}

void Lift::SToG()
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
			if (m_timer < 0)
			{
				m_goTo = false;
				m_timer = 70;
			}
			m_progress = 1.0f;
			m_timer--;
		}
	}
	else
	{
		//引き返している時
		m_progress -= m_speed;
		if (m_progress <= 0.0f)
		{
			if (m_timer < 0)
			{
				m_goTo = true;
				m_timer = 70;
			}
			m_progress = 0.0f;
			m_timer--;
		}
	}
}

void Lift::PointStage()
{
	if (i > m_PointStrike - 1)
	{
		i = m_PointStrike - 1;
	}
	if (i < 0)
	{
		i = 0;
	}
	if (j > m_PointStrike)
	{
		j = m_PointStrike;
	}
	if (j < 0)
	{
		j = 0;
	}

	//スタート地点とゴール地点の座標を取得
	auto& vStart = m_vSave[j];
	auto& vGoal = m_vStage[i];

	//目標地点までのベクトル
	Vector3 vTo = vGoal - vStart;
	//進行具合を加味して現在の地点を割り出す
	Vector3 vNow = vStart + vTo * m_pointprogress[i];
	//地点を中間に合わせる
	m_mWorld.SetTranslation(vNow);

	//進行具合の更新
	if (m_goTo2[i])
	{
		//向かっている時
		m_pointprogress[i] += m_speed;
		if (m_pointprogress[i] >= 1.0f)
		{
			m_goTo2[i] = false;
			m_pointprogress[i] = 1.0f;
			i++;
			j++;
		}
	}
	if (!m_goTo2[i])
	{
		//引き返している時
		m_pointprogress[i] -= m_speed;
		if (m_pointprogress[i] <= 0.0f)
		{
			m_goTo2[i] = true;
			m_pointprogress[i] = 0.0f;
			i--;
			j--;
		}
	}
}

void Lift::XYZRoattion()
{

}

void Lift::NormalLift(const json11::Json& jsonObj)
{
	//移動先情報の読み込み
	if (jsonObj["ToGoal"].is_array())
	{
		auto& p = jsonObj["ToGoal"].array_items();
		m_vGoal = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}
}

void Lift::StageLift(const json11::Json& jsonObj)
{
	if (jsonObj["Points"].is_number())
	{
		m_PointStrike = jsonObj["Points"].number_value();

	}
	for (int i = 0; i < m_PointStrike; i++)
	{
		std::string string = "MoveTo";
		string += std::to_string(i);
		if (jsonObj[string].is_array())
		{
			auto& p = jsonObj[string].array_items();
			m_vStage[i] = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
			m_vSave[0] = m_mWorld.GetTranslation();
			m_vSave[1 + i] = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
			m_goTo2[i] = true;
		}
	}
}