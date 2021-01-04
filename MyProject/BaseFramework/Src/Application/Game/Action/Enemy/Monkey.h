#include "../../GameObject.h"

class Monkey :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新

	Vector3						ResPos;
	Vector3						m_pos;
	Vector3						m_rot;
private:
	enum Faze
	{
		Default,
		Action,
		Attack
	}m_faze = Default;

	void Move();

	void Collision();

	bool Ground(float);

	std::shared_ptr<GameObject> m_spShield;

	float						m_uniqueCol = 20;
	float						m_grabity = 0.3;
	float						m_rotateAngle = 2;
	float						m_speed = 0.05;

	bool						m_isGround;

	bool						m_bShield = true;

	Vector3						m_force;							// キャラの移動量
	Vector3						m_prevPos = {};
};