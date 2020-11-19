#include "../GameObject.h"

class Lift :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj)override;
	virtual void Update()override;
private:
	enum LiftTag
	{
		Normal,
		Point
	};

	// 初期化
	void NormalLift(const json11::Json& jsonObj);
	void StageLift(const json11::Json& jsonObj);

	// 更新
	void SToG();
	void PointStage();
	void XYZRoattion();

	//スタート地点
	Matrix m_mStart;

	//ゴール地点の座標
	Vector3 m_vGoal = {};

	//段階型の座標
	Vector3 m_vStage[11] = {};
	Vector3 m_vSave[12] = {};

	//リフトが進むスピード
	float m_speed = 0.0f;

	LiftTag lifttag;

	int m_PointStrike;

	//リフトが回るスピード
	Vector3 m_vRotSpeed = {};
	Vector3 m_vRot = {};

	//ゴール地点に向かっている割合(0-1)
	float m_progress;
	float m_pointprogress[11];


	//ゴール地点に向かっている状態(true)、引き返している状態(false)
	bool m_goTo = true;
	bool m_goTo2[11];


};