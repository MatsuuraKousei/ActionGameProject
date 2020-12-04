#include "Eagle.h"
#include "../../Scene.h"
#include"../Manage/SpawnManager.h"

void Eagle::Update()
{
	if (!m_alive) { return; }

	// 行動
	switch (m_faze)
	{
	case Default:
		if (m_moveFlg)
		{
			Move();
			m_rot.y += 0.04;
		}
		break;
	case Action:
		break;
	case Attack:
		break;
	}

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Eagle::Move()
{
	//m_force.x = 0.2 * cos(Radians * m_moveAngle) + sm.GetMatrix().GetTranslation().x;
	//m_force.z = 0.2 * sin(Radians * m_moveAngle) + sm.GetMatrix().GetTranslation().z;

	/*m_moveAngle++;

	if (m_moveAngle > 360)
	{
		m_moveAngle = 0;
	}*/



	Vector3 moveVec = m_mWorld.GetAxisZ();
	moveVec.Normalize();

	m_force.x = moveVec.x * 0.3f;
	m_force.z = moveVec.z * 0.3f;


}