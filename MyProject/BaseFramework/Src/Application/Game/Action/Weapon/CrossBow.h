#pragma once

#include"../../GameObject.h"

class CrossBow :public GameObject
{
public:
	void Update()override;
	Vector3 m_pos;												// ワールド行列上の座標
	Vector3 m_rot;												// ワールド行列上の回転角度
private:
	Vector3 m_force;											//キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3 m_prevPos;											// 1フレーム前の座標
};