#include "Item.h"

void Item::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_Rotate = 0.0f;
	m_Trans = { 0,0,0 };
}

void Item::Update()
{
	//m_Rotate += 0.1f;

	m_Trans.y -= 0.01f;

	if (m_Trans.y > 0.05)
	{
		m_Trans *= -1;
	}
	if (m_Trans.y < -0.05)
	{
		m_Trans *= -1;
	}

	m_mWorld.RotateY(m_Rotate);
	m_mWorld.Move(m_Trans);
}
