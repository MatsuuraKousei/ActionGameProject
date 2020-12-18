#pragma once

#include"../../GameObject.h"

class Arrow :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// 初期化
	void Update()override;
	void Position(Vector3);
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }


	Vector3						m_pos;												// ワールド行列上の座標
	Vector3						m_rot;												// ワールド行列上の回転角度
private:
	void CheckBump();
	void Collision();
	void Explosion(const Vector3& hitPos);

	std::weak_ptr<GameObject>	m_wpOwner;				// 発射したオーナーオブジェクト
	Vector3						m_force;				//キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3						m_prevPos;				// 1フレーム前の座標
	int							m_lifeSpan = 90;
	float						m_speed = 0.2f;
	Vector3						m_Axiz;
	float						m_gravity = 0.00f;		//重力の強さ
	bool						m_stop = false;;
};