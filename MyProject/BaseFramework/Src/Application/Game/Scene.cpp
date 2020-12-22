#include "Scene.h"
#include "GameObject.h"
#include "../Component/CameraComponent.h"
#include"../../System/Debug/Debug.h"
#include "./Application/Game/EditorCamera.h"
#include "Application/ImGuiHelper.h"

// コンストラクタ
Scene::Scene()
{

}

// デストラクタ
Scene::~Scene()
{

}

// 初期化
void Scene::Init()
{
	// Jsonファイルを開く
	std::ifstream ifs("Data/JsonFile/test.json");
	if (ifs.fail()) { assert(0 && "Jsonファイルのパスが間違っています"); }

	// 文字列として全読み込み
	// ファイルの最初から最後まで文字コードとして読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0) { assert(0 && "読み込んだファイルのJson変換に失敗"); }

	// 値アクセス
	{
		OutputDebugStringA(jsonObj["Name"].string_value().append("\n").c_str());
		// auto name = jsonObj["Name"].string_value();本来値を取得するならこれだけで良い
		OutputDebugStringA(std::to_string(jsonObj["Hp"].int_value()).append("\n").c_str());
	}

	// 配列アクセス
	{
		auto& pos = jsonObj["Position"].array_items();
		for (auto&& p : pos)
		{
			OutputDebugStringA(std::to_string(p.number_value()).append("\n").c_str());
		}
		//関数化化//////////////////////////////////////////////////////////////////////
		 /*配列添字アクセス
		 OutputDebugStringA(std::to_string(pos[0].number_value()).append("\n").c_str());
		 std::string str;
		 str = std::to_string(pos[0].number_value());
		 str.append("\n");
		 OutputDebugStringA(str.c_str());
		 */
		 ///////////////////////////////////////////////////////////////////////////////
		if (pos.size() == 3)
		{
			// 配列添字アクセス
			OutputDebugStringA(std::to_string(pos[0].number_value()).append("\n").c_str());
			OutputDebugStringA(std::to_string(pos[1].number_value()).append("\n").c_str());
			OutputDebugStringA(std::to_string(pos[2].number_value()).append("\n").c_str());
		}
	}

	// Object取得
	{
		auto& object = jsonObj["monster"].object_items();
		OutputDebugStringA(object["name"].string_value().append("\n").c_str());
		OutputDebugStringA(std::to_string(object["hp"].int_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][0].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][1].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][2].number_value()).append("\n").c_str());
	}

	// Objectの配列取得
	{
		auto& objects = jsonObj["techniques"].array_items();
		for (auto&& object : objects)
		{
			// 共通の要素はチェックなしでアクセス
			OutputDebugStringA(object["name"].string_value().append("\n").c_str());
			OutputDebugStringA(std::to_string(object["atk"].int_value()).append("\n").c_str());
			OutputDebugStringA(std::to_string(object["hitrate"].number_value()).append("\n").c_str());

			// 固有のパラメータはチェックしてからアクセス
			if (object["effect"].is_string())   // 何も入ってなかったらnullが入るのでチェックしやすい
			{
				OutputDebugStringA(object["effect"].string_value().append("\n").c_str());
			}

		}
	}

	// エディターカメラ
	m_spCamera = std::make_shared<EditorCamera>();

	//ImGui
	m_Editor_Log = std::make_shared<CustomImGui>();

	// 文字列格納
	Opning = "Data/JsonFile/Scene/Opning.json";
	Field = "Data/JsonFile/Scene/Field1.json";
	Gameclear = "Data/JsonFile/Scene/Clear.json";
	Gameover = "Data/JsonFile/Scene/Over.json";

	Deserialize();
}

// 初期化
void Scene::Deserialize()
{
	//アクションゲームシーン

	switch (stageProcess)
	{
	case OPNING:
		LoadScene(Opning);
		break;
	case FIELD:
		LoadScene(Field);
		break;
	case CLEAR:
		LoadScene(Gameclear);
		break;
	case OVER:
		LoadScene(Gameover);
		break;
	case DEBUG:
		LoadScene("Data/JsonFile/Scene/Debug.json");
		break;
	default:
		LoadScene("Data/JsonFile/Scene/Field0.json");
		break;
	}
}

// 開放
void Scene::Release()
{
	m_spObjects.clear();
	m_Editor_Log->Clear();
}

// 更新
void Scene::Update()
{
	Debug::GetInstance().Update();

	// カメラ
	if (EditorCameraEnable)
	{
		m_spCamera->Update();
	}

	auto selectObject = m_wpImguiSelectObj.lock();
	// 範囲for(戻り値がポインタ)
	for (auto pObject : m_spObjects)
	{
		if (pObject == selectObject) { continue; }  // 選択されていたら更新しない
		pObject->Update();
	}

	for (auto spObjectItr = m_spObjects.begin(); spObjectItr != m_spObjects.end();/*ここで++しないこと！！*/)// イテレーター(ポインタの配列ver?)
	{
		// missileItrは場所を示している

		// 寿命が尽きていたらリストから除外
		if ((*spObjectItr)->IsAlive() == false)
		{
			// ポインタを削除
			spObjectItr = m_spObjects.erase(spObjectItr);
			// この時点で次の部分に行っている？
		}
		else
		{
			// eraseを通った時は++しないようにすること　バグるよ
			++spObjectItr;
		}
	}

	// シーン遷移のリクエストがあった場合、変更する
	if (m_isRequestChangeScene)
	{
		ExecChangeScene();
	}


	// 死亡してる？
	for (auto pObject : m_spObjects)
	{
		if (pObject->m_Hp != 0) { continue; }
		if (pObject->m_Hp == 0)
		{
			pObject->m_alive = false;
		}
	}
}

// 描画
void Scene::Draw()
{
	// カメラのセット
	if (EditorCameraEnable)
	{
		m_spCamera->SetToShader();
	}
	else
	{
		// 消されないようにロック
		std::shared_ptr<CameraComponent> spCamera = m_wpTargetCamera.lock();
		if (spCamera)
		{
			spCamera->SetToShader();
		}
	}


	// ライトの情報をセット
	SHADER.m_cb8_Light.Write();

	// エフェクトシェーダーを描画デバイスにセット
	SHADER.m_effectShader.SetToDevice();


	// 不透明物描画
	SHADER.m_standardShader.SetToDevice();


	// 範囲for(戻り値がポインタ)
	for (auto pObject : m_spObjects)
	{
		if (pObject->IsAlive())
		{
			pObject->Draw();
		}
	}

	// 半透明物描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());


	// Z情報は使うが、Z書き込みOFF
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
	// カリング無し(両面描画)
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);


	// 終わったら元に戻す
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);



	// デバッグライン描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
	{
		// 白線
		//AddDebugLine(Math::Vector3(), Math::Vector3(0.0f, 10.0f, 0.0f));

		// 球体
		//AddDebugSphereLine(Math::Vector3(5.0f, 5.0f, 0.0f), 2.0f);

		// 軸
		//AddDebugCoordinateAxisLine(Math::Vector3(0.0f, 5.0f, 5.0f), 3.0f);

		// Zバッファ使用OFF・書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

		if (debug)
		{
			// 点が２つ以上ある時に描画する
			if (Debug::GetInstance().debugLines.size() >= 1)
			{
				SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());

				SHADER.m_effectShader.DrawVertices(Debug::GetInstance().debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

				// ここでクリアしないと前に書いたものが残る?
				Debug::GetInstance().debugLines.clear();
			}
		}

		//D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);


		for (auto spObj : m_spObjects)
		{
			spObj->DrawEffect();
		}

		// 終わったら元通りにする
		// Zバッファ使用ON・書き込みON
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	}
	// 全てのオブジェクトの2D描画を行う
	for (auto obj : m_spObjects)
	{
		obj->Draw2D();
	}

}

// シーンのロード
void Scene::LoadScene(const std::string& sceneFilename)
{
	Reset();        // 各項目のクリア

	// JSON読み込み
	json11::Json json = ResFac.GetJSON(sceneFilename);
	if (json.is_null())
	{
		assert(0 && "[LoadScene]jsonファイル読み込み失敗");
		return;
	}

	// オブジェクトリスト所得
	auto& objectDataList = json["GameObjects"].array_items();

	// オブジェクトリスト所得
	for (auto&& objJsonData : objectDataList)   // 右辺値参照、左辺値参照で調べればauto&&について出るらしい
	{
		// オブジェクト作成
		auto newGameObj = CreateGameObject(objJsonData["ClassName"].string_value());

		// プレハブ指定ありの場合は、プレハブ側のものをベースにこのJSONをマージする
		MergePrefab(objJsonData);

		// オブジェクトのデシリアライズ
		newGameObj->Deserialize(objJsonData);

		// リストへ追加
		AddObject(newGameObj);
	}
}

// シーン変更のリクエストを受け付け
void Scene::RequestChangeScene(const std::string& filename)
{
	// 次のシーンのJsonファイル名を覚える
	m_nextSCeneFileName = filename;

	// リクエストがあったことを覚える
	m_isRequestChangeScene = true;
}

// シーンを実際に変更するところ
void Scene::ExecChangeScene()
{
	LoadScene(m_nextSCeneFileName); // シーンの遷移

	m_isRequestChangeScene = false; // リクエスト状況のリセット
}

// シーンをまたぐ時にリセットする処理
void Scene::Reset()
{
	m_spObjects.clear();            // メインのリストをクリア
	m_wpImguiSelectObj.reset();     // ImGuiが選んでいるオブジェクトをクリア
	m_wpTargetCamera.reset();       // カメラのターゲットになっているキャラクタのリセット
	//m_spSky = nullptr;              // 空のクリア
}

// ImGuiの更新
void Scene::ImGuiUpdate()
{
	// ロック
	auto selectObject = m_wpImguiSelectObj.lock();

	if (ImGui::Begin("Scene"))
	{
		//          ↓utf-8に変換する
		//ImGui::Text(u8"今日はいい天気だから\n飛行機の座標でも表示しようかな");
		//ImGui::Checkbox("EditorCamera", &EditorCameraEnable); // チェックボックスで変更できるようになる

		// オブジェクトリストの描画
		if (ImGui::CollapsingHeader("Object List", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto&& rObj : m_spObjects)
			{
				// 選択オブジェクトと一致するオブジェクトかどうか
				bool selected = (rObj == selectObject);

				ImGui::PushID(rObj.get());

				// Selectableで選択できるようになる 第二引数はハイライトするかしないか
				if (ImGui::Selectable(rObj->Getname(), selected))
				{
					m_wpImguiSelectObj = rObj;
				}

				ImGui::PopID();
			}
		}
	}
	ImGui::End();

	// インスペクターウィンドウ
	if (ImGui::Begin("Inseprctor"))
	{
		if (selectObject)
		{
			// オブジェクトリストで選択したゲームオブジェクトの情報を描画
			selectObject->ImGuiUpdate();
		}
	}
	ImGui::End();

	ImGuiPrefabFactoryUpdate();
}

// 課題用//////////////////////////////////////////////////////////////////////////////////////////////
void Scene::ImGuiPrefabFactoryUpdate()
{
	// ログ
	m_Editor_Log->ImGuiUpdate("LogWindow");

	// PrefabFactory
	if (ImGui::Begin("PrefabFactory"))
	{
		// jsonのパスの読み込み
		ImGui::InputText("", &m_PathText);

		ImGui::SameLine();      // 次を同じ行にしたいとき

		// Jsonパス取得ボタン
		if (ImGui::Button(u8"ファイル選択"))
		{
			// エクスプローラーを開いてファイルを選択
			if (Window::OpenFileDialog(m_PathText, "Jsonファイルを選択してください", "\0*.json"))
			{
				//選択したファイルのパスをログに表示
				m_Editor_Log->AddLog(u8"読み込んだJsonファイルのパス = %s", m_PathText.c_str());
			}
		}

		// Createボタン
		if (ImGui::Button("Create"))
		{
			json11::Json json = ResFac.GetJSON(m_PathText);

			// 成功
			if (!json.is_null())
			{
				m_Editor_Log->AddLog(u8"%s 生成成功", m_PathText.c_str());
			}
			// 失敗
			else
			{
				m_Editor_Log->AddLog(u8"%s 生成失敗", m_PathText.c_str());
				return;
			}

			// クラス名からオブジェクトを追加
			std::shared_ptr<GameObject> gm = CreateGameObject(json["ClassName"].string_value());

			// Prefabのマージ処理
			MergePrefab(json);

			// 初期化
			gm->Deserialize(json);

			// リストに追加
			AddObject(gm);
		}
	}
	ImGui::End();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

// オブジェクトの追加
void Scene::AddObject(std::shared_ptr<GameObject> spObject)
{
	if (spObject == nullptr) { return; }
	m_spObjects.push_back(spObject);
}

std::shared_ptr<GameObject> Scene::FindObjectWithName(const std::string& name)
{
	//文字列比較
	for (auto&& obj : m_spObjects)
	{
		if (obj->GetName() == name) { return obj; }
	}

	//見つからなかったらnullが帰る
	return nullptr;
}
Vector3 Scene::ConvertScreenToWorld(int sx, int sy, float fZ, int screen_w, int screen_h, const Matrix& rView, const Matrix& rPrj)
{
	Matrix invView = rView;
	invView.Inverse();
	Matrix invPrj = rPrj;
	invPrj.Inverse();

	Matrix viewPort;

	viewPort._11 = screen_w / 2.0;
	viewPort._22 = -screen_h / 2.0;
	viewPort._41 = screen_w / 2.0;
	viewPort._42 = screen_h / 2.0;

	Matrix invViewport = viewPort;

	invViewport.Inverse();


	//2D→3D座標への変換行列
	Matrix convertMat = invViewport * invPrj * invView;


	Vector3 resultPos((float)sx, (float)sy, (float)fZ);


	return resultPos.TransformCoord(convertMat);
}