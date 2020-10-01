﻿#pragma once

#include "../GameObject.h"

class Human :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

	//着地しているかどうか
	bool IsGround() { return m_isGround; }

private:
	void UpdateMove();	//操作・キャラの行動による移動

	float m_moveSpeed = 0.1f;	//キャラの移動速度
	KdVec3 m_pos;			//ワールド行列上の座標

	void UpdateCamera();	//操作によるカメラの回転と移動
	float m_camRotSpeed = 0.2f;	//カメラの回転速度

	void UpdateRotate(const KdVec3& rMoveDir);	//操作やキャラクターの行動による回転計算
	float m_rotateAngle = 10.0f;				//キャラクターの回転角度
	KdVec3 m_rot;								//ワールド行列上の回転角度

	float m_gravity = 0.01f;	//重力の強さ
	KdVec3 m_force;				//キャラクターにかかる移動させる力(落下、跳躍、移動)

	static const float s_allowToStepHeight;		//歩いて乗り越えられる段差の高さ
	static const float s_landingHeight;			//地面から足が離れていても着地していると判定する高さ（坂道などを下るときに宙に浮くのを避ける）

	void UpdateCollision();						//当たり判定全般
	bool CheckGround(float& rDstDistance);		//地面との判定

	KdVec3 m_prevPos;							//1フレーム前の座標
	bool m_isGround;							//着地しているかどうか

	std::shared_ptr<KdAnimationData>	m_spAnimation = nullptr;	//再生するアニメーションデータ
	float	m_animationTime = 0.0f;	//再生中のアニメーション位置

	bool Edit = false;
	LPPOINT lp;
	POINT p;
};