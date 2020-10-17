#pragma once

// 前方宣言
class CameraComponent;
class InputComponent;
class ModelComponent;

struct SphereInfo;
struct RayInfo;

// タグ定数
enum OBJECT_TAG
{
	// 0x は16進数
	TAG_None = 0x00000000,	// 属性なし：初期設定
	TAG_Character = 0x00000001,	// キャラクター設定
	TAG_Player = 0x00000002,	// プレイヤー属性
	TAG_StageObject = 0x00000004,	// 背景オブジェクト属性
	TAG_AttackHit = 0x00000010,	// 攻撃をくらう属性(16) 
	TAG_Enemy = 0x00000020,	// 敵
	TAG_DamegeObject = 0x00000100,	// ダメージオブジェクト
};

struct Rot
{
	int x;
	int y;
	int z;
};

class GameObject : public std::enable_shared_from_this<GameObject>	// thisポインタをshared_ptrに変換する
{
public:

	GameObject();		// コンストラクタ
	~GameObject();		// デストラクタ

	// virtualを付けることでそのまま使うことも上書きすることもできる(仮想関数)
	virtual void Deserialize(const json11::Json& jsonObj);	// 初期化
	virtual void Update();		// 更新
	virtual void Draw();		// 描画

	inline const std::string& GetName()const { return m_name; }

	virtual void DrawEffect() {}	// 半透明物の描画

	virtual void Draw2D() {}

	virtual void ImGuiUpdate();	// ImGuiの処理

	inline const Matrix& GetMatrix()const { return m_mWorld; }
	inline void SetMatrix(const Matrix& rMat) { m_mWorld = rMat; }
	// カプセル化
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }

	inline void SetTag(UINT tag) { m_tag = tag; }
	inline UINT GetTag() const { return m_tag; }

	inline const char* Getname() const { return m_name.c_str(); }

	// 球による当たり判定(距離)
	bool HitCheckBySphere(const SphereInfo& rInfo);

	// レイによる当たり判定
	bool HitCheckByRay(const RayInfo& rInfo, RayResult& rResult);

	// カメラコンポーネント取得
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }
	// なんか先生の動画にうつってたから====================
	// 入力コンポーネント取得
	std::shared_ptr<InputComponent> GetInputComponent() { return m_spInputComponent; }
	// モデルコンポーネント取得
	std::shared_ptr<ModelComponent> GetModelComponent() { return m_spModelComponent; }

	const Matrix& GetPrevMatrix() { return m_mPrev; }

	//このキャラクターが動いた分の行列を取得
	Matrix GetOneMove()
	{
		Matrix mPI = m_mPrev;
		mPI.Inverse();			//動く前の逆行列
		return mPI * m_mWorld;	//動く前の逆行列*今の行列=一回動いた分の行列
	}

	int				m_Hp = 1;						// ヒットポイント

	bool			m_alive = true;			// 生きているか死んでいるか
protected:
	virtual void Release();		// 開放

	// 本来はここでインスタンス化するのはよくないらしい
	// 各オブジェクトの中でインスタンスとかしたほうがいい？
	// カメラコンポーネント
	// 初期化でオーナーを渡す(コンストラクタでそう書いたから)
	// shared_ptr化した変数は自動でdeleteしてくれる											　↓GameObject型
	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	// インプットコンポーネント
	std::shared_ptr<InputComponent> m_spInputComponent = std::make_shared<InputComponent>(*this);
	// モデルコンポーネント
	std::shared_ptr<ModelComponent> m_spModelComponent = std::make_shared<ModelComponent>(*this);

	Matrix		m_mWorld;					// ゲーム内の絶対座標(座標と回転と拡大)

	Matrix		m_mPrev;					//動く前の行列

	Rot				m_rotation;					// 回転
	UINT			m_tag = OBJECT_TAG::TAG_None;
	std::string		m_name = "GameObject";		// 名前

	float			m_colRadius = 2.0f;			// このキャラクターの半径

};

// クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

// 球判定に使うデータ
struct SphereInfo
{
	Vector3	m_pos = {};
	float	m_radius = 0.0f;
};

struct RayInfo
{
	Vector3		m_pos;				// レイ(光線)の発射場所
	Vector3		m_dir;				// レイの発射方向
	float		m_maxRange = 0.0f;	// レイが届く最大距離
};