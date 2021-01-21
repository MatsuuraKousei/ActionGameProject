#pragma once

#include"../../GameObject.h"

class Cutter :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
private:
	Vector3						m_prevPos;								// １フレーム前の座標
	Vector3						m_pos;									// ワールド行列上の座標
	Vector3						m_rot;									// キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3						m_force;								// ワールド行列上の回転角度
};