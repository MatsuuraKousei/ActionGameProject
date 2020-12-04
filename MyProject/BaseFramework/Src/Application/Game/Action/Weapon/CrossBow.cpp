#include "CrossBow.h"

void CrossBow::Update()
{
	//移動前の座標を覚える
	m_prevPos = m_pos;


	//ワールド行列の合成する
	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.Move(m_pos);
}
