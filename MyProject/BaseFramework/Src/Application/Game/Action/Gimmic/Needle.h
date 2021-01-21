#pragma once

#include "../../GameObject.h"

class Needle :public GameObject
{

public:
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新
private:

	Vector3						m_prevPos;								// １フレーム前の座標
	Vector3						m_pos;									// ワールド行列上の座標
	Vector3						m_rot;									// キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3						m_force;								// ワールド行列上の回転角度

		//スタート地点
	Matrix m_mStart;

	//ゴール地点の座標
	Vector3 m_vGoal = {};
	//リフトが進むスピード
	float m_speed = 0.0f;

	//ゴール地点に向かっている状態(true)、引き返している状態(false)
	bool m_goTo = true;

	//リフトが回るスピード
	Vector3 m_vRotSpeed = {};

	//ゴール地点に向かっている割合(0-1)
	float m_progress;
};