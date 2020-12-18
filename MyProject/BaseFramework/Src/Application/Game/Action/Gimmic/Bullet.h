#pragma once

#include "../../GameObject.h"

class Bullet :public GameObject
{
public:

	virtual void Deserialize(const json11::Json& jsonObj)override;		// ������
	virtual void Update()override;										// �X�V

	void GetGimmicAngle(Vector3);

	// �������L�����N�^�[��o�^����
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }
private:

	Vector3						m_prevPos;	// �P�t���[���O�̍��W

	std::weak_ptr<GameObject>	m_wpOwner;	// ���˂����I�[�i�[�I�u�W�F�N�g

	Vector3			m_pos;												// ���[���h�s���̍��W
	Vector3			m_rot;												// �L�����N�^�[�ɂ�����ړ��������(�����A����A�ړ�)
	Vector3			m_force;											// ���[���h�s���̉�]�p�x

	int				m_lifeSpan = 0;
	float			m_speed = 0.3f;
	Vector3         m_GimmicAngle;
};