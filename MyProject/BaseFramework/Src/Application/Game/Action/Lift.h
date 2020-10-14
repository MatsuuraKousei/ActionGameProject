#include "../GameObject.h"

class Lift :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;

private:

	//スタート地点
	Matrix m_mStart;

	//ゴール地点の座標
	Vector3 m_vGoal = {};

	//リフトが進むスピード
	float m_speed = 0.0f;

	//リフトが回るスピード
	Vector3 m_vRotSpeed = {};
	Vector3 m_vRot = {};

	//ゴール地点に向かっている割合(0-1)
	float m_progress = 0.0f;

	//ゴール地点に向かっている状態(true)、引き返している状態(false)
	bool m_goTo = true;

	
};