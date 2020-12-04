#include "BatBullet.h"
#include "../../../main.h"
#include"../../Scene.h"

void BatBullet::Deserialize(const json11::Json& jsonObj)
{
	// ���������瑁�����^�[��
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// �����鎞��(�ő�Fps�Ɉˑ�?)
	m_lifeSpan = APP.m_maxFps * 2;
}

void BatBullet::Update()
{
	if (Scene::GetInstance().debug) { return; }
	// ����ł���ꍇ�������^�[��
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		// �������ǂ�����Scene������
		Destroy();
	}


	auto target = m_wpTarget.lock();

	if (target)
	{
		if (!m_targetting)
		{
			// ���g����^�[�Q�b�g�ւ̃x�N�g��
			Vector3 vTarget = target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();


			// �P�ʃx�N�g�����F���g����^�[�Q�b�g�֌������Ȃ����P�̃x�N�g��
			vTarget.Normalize();

			// ������Z����(�O����)
			Vector3 vZ = m_mWorld.GetAxisZ();

			// �g�嗦�������Ă���ƌv�Z�����������Ȃ邽�ߒP�ʃx�N�g����
			vZ.Normalize();

			// ������������]���쐬(���̎��ŉ�]����)����������
			Vector3 vRotAxis = Vector3::Cross(vZ, vTarget);

			// 0�x�N�g���Ȃ��]���Ȃ�
			if (vRotAxis.LengthSquared() != 0)
			{
				// ������Z�����x�N�g���Ǝ��g����^�[�Q�b�g�֌������x�N�g���̓���
				float d = Vector3::Dot(vZ, vTarget);

				// �덷��-1�`1�ȏ�ɂȂ�\����Ȃ̂ŁA�N�����v����(std::clamp�ł���)
				if (d > 1.0f)d = 1.0f;
				else if (d < -1.0f)d = -1.0f;

				// �����̑O�����x�N�g���Ǝ��g����^�[�Q�b�g�֌������x�N�g���Ԃ̊p�x(radian)
				float radian = acos(d);	// ���W�A���p�ɕϊ�

				// �p�x�����@�P�t���[���ɂ��ő�łP�x�ȏ��]���Ȃ�
				if (radian > 1.0f * Radians)
				{
					radian = 1.0f * Radians;
				}

				// ����������radian �i�����܂łŉ�]�p�x�����܂����j����������


				Matrix mRot;
				mRot.CreateRotationAxis(vRotAxis, radian);

				//m_mWorld = mRot * m_mWorld;
				// �C��
				auto pos = m_mWorld.GetTranslation();
				m_mWorld.SetTranslation({ 0,0,0 });
				m_mWorld *= mRot;
				m_mWorld.SetTranslation(pos);
			}
		}

		Vector3 move = m_mWorld.GetAxisZ();
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

		m_time--;
		if (m_time < 0)
		{
			m_targetting = true;
		}
	}
}