#pragma once

#include "../GameObject.h"
#include "../../Component/AudioComponent.h"

class Arrow;
class CrossBow;
class Sword;

struct Lockoninjector;

class Human :public GameObject
{
public:
	struct Lockoninjector
	{
		bool lockon;
		Math::Vector2 vec;
	};
	virtual void Deserialize(const json11::Json& jsonObj)override;// 初期化

	virtual void Update()override;								  // 更新

	void DrawEffect() override;		// 透明物描画

	void Explosion(const Vector3&);

	//着地しているかどうか
	bool IsGround() { return m_isGround; }

	static bool& IsSnipe()
	{
		static bool snipe;
		return snipe;
	}

	static Lockoninjector& IsLockOn()
	{
		static Lockoninjector lock;
		return lock;
	}

	static Vector3& IsKnockBack()
	{
		static Vector3 KnockBack;
		return KnockBack;
	}


private:

	void UpdateMove();											//操作・キャラの行動による移動

	void Lockon();
	float f;
	std::weak_ptr<GameObject> m_wpTarget;

	void SwordInit();											// 剣初期化
	void SwordUpdate();											// 剣更新
	void CrossbowInit();										// クロスボウ初期化
	void CrossbowUpdate();										// クロスボウ更新

	Vector3 m_memory;

	float m_moveSpeed = 0.2f;									// キャラの移動速度
	Vector3 m_pos;												// ワールド行列上の座標

	void UpdateCamera();										// 操作によるカメラの回転と移動
	float m_camRotSpeed = 0.2f;									// カメラの回転速度

	void UpdateRotate(const Vector3& rMoveDir);					// 操作やキャラクターの行動による回転計算
	float m_rotateAngle = 10.0f;								// キャラクターの回転角度
	Vector3 m_rot;												// ワールド行列上の回転角度

	float m_gravity = 0.03f;									//重力の強さ
	Vector3 m_force;											//キャラクターにかかる移動させる力(落下、跳躍、移動)

	static const float s_allowToStepHeight;						// 歩いて乗り越えられる段差の高さ
	static const float s_landingHeight;							// 地面から足が離れていても着地していると判定する高さ（坂道などを下るときに宙に浮くのを避ける）

	void UpdateCollision();										// 当たり判定全般
	bool CheckGround(float& rDstDistance);						// 地面との判定
	Vector3 CheckBump(Vector3);									// 三角ポリゴンとの当たり判定

	void Damege();

	Vector3 m_prevPos;											// 1フレーム前の座標
	bool m_isGround;											// 着地しているかどうか
	bool m_isObject;											// 着地しているかどうか
	bool m_isWall;												// 壁に当たっているかどうか

	bool m_bSword;

	bool m_gravityFlg = true;

	Vector3 moveVec;

	std::vector<EffectShader::Vertex> m_debugLines;
	float						m_uniqueCol = 3.0f;

	bool b = false;

	bool m_damegeStayFlg = false;								// ダメージ受付
	int m_damegeStayTime = 20;								// ダメージ無敵時間

	std::shared_ptr<CrossBow>	m_spCrossbow = nullptr;
	std::shared_ptr<GameObject> m_spCBCamera = nullptr;
	std::shared_ptr<Sword>		m_spSword = nullptr;

	Model::Node* ArmL;
	Model::Node* Body;
	Model::Node* Head;
	Model::Node* Leg;

	float						m_fswordInitAngle = 0.0f;
	bool						m_bswordAttck = false;

	std::shared_ptr<AnimationData>	m_spAnimation = nullptr;	//再生するアニメーションデータ
	float	m_animationTime = 0.0f;								//再生中のアニメーション位置

	std::vector<TrailPolygon>	m_swordTrail;	// 軌跡

	float  m_fEffectTime = 20;

	Vector3 MaxRange;

	float m_fBodyInit;

	Vector3 m_CameraTrans;

	bool Edit = true;;
	LPPOINT lp;
	POINT p;

	void LockonPoint(const Vector3& hitPos);
	Vector3 m_enemypos;

	std::shared_ptr<SoundInstance> m_spBowSE;
	std::shared_ptr<SoundInstance> m_spJumpSE;
	
};

