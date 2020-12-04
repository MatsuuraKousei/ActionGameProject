#pragma once

#include "../../GameObject.h"

class BatBullet :public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;		// ������
	virtual void Update()override;										// �X�V

	// �������L�����N�^�[��o�^����
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }

	// �^�[�Q�b�g
	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }
private:

	Vector3						m_prevPos;	// �P�t���[���O�̍��W

	std::weak_ptr<GameObject>	m_wpOwner;	// ���˂����I�[�i�[�I�u�W�F�N�g

	std::weak_ptr<GameObject>	m_wpTarget;

	Vector3			m_pos;												// ���[���h�s���̍��W
	Vector3			m_rot;												// �L�����N�^�[�ɂ�����ړ��������(�����A����A�ړ�)
	Vector3			m_force;											// ���[���h�s���̉�]�p�x

	bool            m_targetting = false;

	int				m_time = 10;

	int				m_lifeSpan = 0;
	float				m_speed = 0.3f;
};