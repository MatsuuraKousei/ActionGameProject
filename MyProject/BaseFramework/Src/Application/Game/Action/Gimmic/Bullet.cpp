#include "Bullet.h"
#include"../../Scene.h"
#include "../../../main.h"

void Bullet::Deserialize(const json11::Json& jsonObj)
{
	// ���������瑁�����^�[��
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// �����鎞��(�ő�Fps�Ɉˑ�?)
	m_lifeSpan = APP.m_maxFps * 10;
}

void Bullet::Update()
{
	// ����ł���ꍇ�������^�[��
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		// �������ǂ�����Scene������
		Destroy();
	}

	// �ς���O�ɍ��W��ۑ�
	m_prevPos = m_mWorld.GetTranslation();

	Vector3 move = m_mWorld.GetAxisZ() + m_GimmicAngle;
	move.Normalize();

	move *= (float)m_speed;

	m_mWorld.Move(move);

	// �����̍쐬
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 0.01;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// �������g�𖳎�
		if (obj.get() == this) { continue; }

		// �L�����N�^�[�Ɠ����蔻�������̂ł���ȊO�͖���
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// �����蔻��
		if (obj->HitCheckBySphere(info))
		{
			obj->m_Hp--;
			Destroy();
		}
	}

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//�������g�͖���
		if (obj.get() == this) { continue; }
		//�X�e�[�W�Ɠ����蔻��i�w�i�I�u�W�F�N�g�ȊO�ɏ��Ƃ��͕ύX�̉\������j
		if (obj->GetTag() & TAG_StageObject)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				Destroy();
			}
		}
		if (obj->GetTag() & TAG_ActiveObject)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				Destroy();
			}
		}
	}
}

void Bullet::GetGimmicAngle(Vector3 vec)
{
	m_GimmicAngle = vec;
}