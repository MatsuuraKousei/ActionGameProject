#pragma once

#include "../../GameObject.h"

class Bullet :public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	virtual void Update()override;										// 更新

	void GetGimmicAngle(Vector3);

	// 撃ったキャラクターを登録する
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }
private:

	Vector3						m_prevPos;	// １フレーム前の座標

	std::weak_ptr<GameObject>	m_wpOwner;	// 発射したオーナーオブジェクト

	Vector3			m_pos;												// ワールド行列上の座標
	Vector3			m_rot;												// キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3			m_force;											// ワールド行列上の回転角度

	int				m_lifeSpan = 0;
	float			m_speed = 0.3f;
	Vector3         m_GimmicAngle;
};