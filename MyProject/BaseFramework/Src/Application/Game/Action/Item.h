#pragma once

#include "../GameObject.h"
#include "../../Component/AudioComponent.h"

class Item :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	bool						m_moveFlg = false;
	int							m_Max;

	Vector3						m_prevPos;	// １フレーム前の座標
	Vector3						m_pos;		// ワールド行列上の座標
	Vector3						m_rot;		// キャラクターにかかる移動させる力(落下、跳躍、移動)
	Vector3						m_force;	// ワールド行列上の回転角度


	// アイテム回転
	float			m_Rotate;
	// アイテム上下
	Vector3			m_Trans;

	std::shared_ptr<SoundInstance> m_spGetSE;
};