#include "Door.h"
#include"../../Scene.h"

void Door::Deserialize(const json11::Json& jsonObj)
{
	// ���������瑁�����^�[��
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_Uping = false;
	
}

void Door::Update()
{
	if (m_Uping)
	{
		if (m_mWorld.GetTranslation().y<10)
		{
			m_force.y += 0.5f;
		}
	}

	// �ς���O�ɍ��W��ۑ�
	m_prevPos = m_mWorld.GetTranslation();

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.Move(m_pos);
}

void Door::Switch(bool b)
{
	m_Uping = b;
}
