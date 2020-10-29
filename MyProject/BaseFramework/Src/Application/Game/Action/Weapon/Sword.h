#pragma once

#include"../../GameObject.h"

class Sword :public GameObject
{
public:
	
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }

private:

	std::weak_ptr<GameObject>	m_wpOwner;	// 発射したオーナーオブジェクト
	Vector3						m_prevPos;	// １フレーム前の座標
	Vector3						m_pos;		// ワールド行列上の座標
	Vector3						m_rot;		// キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3						m_force;	// ワールド行列上の回転角度
};