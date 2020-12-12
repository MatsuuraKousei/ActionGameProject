#pragma once	// 多重インクルード防止

// 前方宣言
//	ヘッダーの中では絶対にインクルードしないとだめって場合じゃない時は前方宣言のほうがいい
class EditorCamera;
class GameObject;
class CameraComponent;
class CustomImGui;

enum StageProcess
{
	OPNING,
	FIELD,
	CLEAR,
	OVER,
	DEBUG
};

class Scene
{
public:
	// シングルトン
	static Scene& GetInstance()
	{
		// ゲームのおおもとに入れる？
		static Scene instance;
		return instance;
	}

	~Scene();		// デストラクタ

	void Init();	// 初期化
	void Deserialize();
	void Release();	// 開放
	void Update();	// 更新
	void Draw();	// 描画

	// 一気にシーンを変えるのではなくワンクッションおく
	void RequestChangeScene(const std::string& filename);	// シーン変更のリクエストを受け付け

	/////////////////////////////////////////////////
	void ImGuiPrefabFactoryUpdate();	// (課題用)
	/////////////////////////////////////////////////

	void ImGuiUpdate();	// ImGuiの更新

	void AddObject(std::shared_ptr<GameObject> spObject);	// 渡されたミサイルを追加

	//指定された名前で検索をかけて合致した最初のオブジェクトを返す
	std::shared_ptr<GameObject> FindObjectWithName(const std::string& name);

	// GameObjectのリストを返す
	const std::list<std::shared_ptr<GameObject>> GetObjects()const { return m_spObjects; }

	inline void SetTargetCamera(std::shared_ptr<CameraComponent> spCamera) { m_wpTargetCamera = spCamera; }

	bool							EditorCameraEnable = true;			// true:世界 false:プレイヤー

	bool							debug = false;

	StageProcess					stageProcess = FIELD;

	std::string						Opning;
	std::string						Field;
	std::string						Gameover;
	std::string						Gameclear;

	Vector3 ConvertScreenToWorld(int sx, int sy, float fZ, int screen_w, int screen_h, const Matrix& rView, const Matrix& rPrj);

private:

	// メンバ関数=============================================================================
	Scene();		// コンストラクタ

	void LoadScene(const std::string& sceneFilename);	// シーンのロード

	void ExecChangeScene();								// シーンを実際に変更するところ
	void Reset();										// シーンをまたぐ時にリセットする処理

	// メンバ変数=============================================================================
	std::string m_nextSCeneFileName = "";				// 次のシーンのJsonファイル名
	bool m_isRequestChangeScene = false;				// シーン遷移のリクエストがあったか

	std::shared_ptr<EditorCamera>	m_spCamera = nullptr;		// 地面


	// vectorは横並びだけどlistはバラバラなものを線でつないでいるイメージ
	// シェアードポインタ(代入されたときにカウンタが増えたり？)
	std::list<std::shared_ptr<GameObject>> m_spObjects;


	// Imguiで選択されたオブジェクト
	std::weak_ptr<GameObject> m_wpImguiSelectObj;

	// ターゲットカメラ
	std::weak_ptr<CameraComponent> m_wpTargetCamera;

	



	//ImGui用の課題変数/////////////////////////////////////////////////////////////
	std::string							m_PathText;	// ImGuiのjsonのパスを入れる
	std::shared_ptr<CustomImGui>		m_Editor_Log;	//ImGuiHelperのポインタ
	////////////////////////////////////////////////////////////////////////////////
};