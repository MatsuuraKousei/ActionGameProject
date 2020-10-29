#pragma once

#include "../../GameObject.h"

class Bullet;

class ShotGimmic :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// ������
	virtual void Update()override;										// �X�V

private:
	void BulletBorn();

	Vector3						m_prevPos;								// �P�t���[���O�̍��W
	Vector3						m_pos;									// ���[���h�s���̍��W
	Vector3						m_rot;									// �L�����N�^�[�ɂ�����ړ��������(�����A����A�ړ�)
	Vector3						m_force;								// ���[���h�s���̉�]�p�x

	int time = 1;
};