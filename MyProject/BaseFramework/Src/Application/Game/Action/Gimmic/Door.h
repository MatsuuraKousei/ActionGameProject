#pragma once

#include "../../GameObject.h"


class Door :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// ������
	virtual void Update()override;										// �X�V

	void	Switch(bool);
	Vector3						m_pos;									// ���[���h�s���̍��W
	Vector3						m_rot;									// �L�����N�^�[�ɂ�����ړ��������(�����A����A�ړ�)
private:

	Vector3						m_prevPos;								// �P�t���[���O�̍��W
	Vector3						m_force;								// ���[���h�s���̉�]�p�x

	bool						m_Uping = false;

};