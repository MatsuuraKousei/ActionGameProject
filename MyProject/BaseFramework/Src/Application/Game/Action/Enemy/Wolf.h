#pragma once

#include "Boss.h"

class Wolf :public Boss
{
public:

	virtual void Update()override;

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	Vector3						m_pos;

private:
	void Move();

	void Snipe();

	void UpdateCollision();

	void VectorMove(Matrix);

	
};