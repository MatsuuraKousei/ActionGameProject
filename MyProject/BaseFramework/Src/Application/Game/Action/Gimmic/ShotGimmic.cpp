#include "ShotGimmic.h"
#include"Bullet.h"
#include"../../Scene.h"

void ShotGimmic::Deserialize(const json11::Json& jsonObj)
{
	// ���������瑁�����^�[��
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);
}

void ShotGimmic::Update()
{
	// �ς���O�ɍ��W��ۑ�
	m_prevPos = m_mWorld.GetTranslation();

	time--;
	if (time < 0)
	{
		BulletBorn();
		time = 1;
	}

	m_rot.y += 0.05f;


	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void ShotGimmic::BulletBorn()
{
	std::shared_ptr<Bullet>		m_spBullet = std::make_shared<Bullet>();

	m_spBullet->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Bullet.json"));

	m_spBullet->GetGimmicAngle(m_rot.y);

	m_spBullet->SetMatrix(m_mWorld);

	m_spBullet->SetOwner(shared_from_this());

	Scene::GetInstance().AddObject(m_spBullet);
}
