#pragma once

#include "../../GameObject.h"

class Door;

class Target :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;		// ������
	virtual void Update()override;										// �X�V

	void BornDoor();

private:

	Vector3						m_prevPos;								// �P�t���[���O�̍��W
	Vector3						m_pos;									// ���[���h�s���̍��W
	Vector3						m_rot;									// �L�����N�^�[�ɂ�����ړ��������(�����A����A�ړ�)
	Vector3						m_force;								// ���[���h�s���̉�]�p�x

	std::shared_ptr<Door>		m_spDoor = nullptr;
	Vector3						m_DoorPos[10];
	Vector3						m_DoorAngle[10];

	int							m_DoorNum;

	bool						m_Switch;
};