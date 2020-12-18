#include "GameObject.h"
#include"SceneManage.h"
#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"
#include "../Component/ModelComponent.h"
#include"SceneCameraManage.h"
#include"Action/Manage/SpawnManager.h"
#include "Action/ActionGameProcess.h"
#include "Action/Human.h"
#include"Action/Lift.h"
#include"Action/Enemy/Boar.h"
#include"Action/Enemy/Alligator.h"
#include"Action/Enemy/Eagle.h"
#include"Action/Enemy/Bat.h"
#include"Action/Gimmic/Target.h"
#include"Action/Item.h"
#include "Action/Gimmic/ShotGimmic.h"

// コンストラクタ
GameObject::GameObject()
{
}

// デストラクタ
GameObject::~GameObject()
{
	Release();
}

// 初期化
void GameObject::Deserialize(const json11::Json& jsonObj)
{
	if (jsonObj.is_null()) { return; }

	if (jsonObj["Name"].is_null() == false)
	{
		m_name = jsonObj["Name"].string_value();
	}

	// タグ
	if (jsonObj["Tag"].is_null() == false)
	{
		m_tag = jsonObj["Tag"].int_value();
	}

	// モデル---------------------------------------

	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(ResFac.GetModel(jsonObj["ModelFileName"].string_value()));
	}
	// 行列---------------------------------------
	Matrix mTrans, mRotate, mScale;

	// 座標
	const std::vector<json11::Json>& rPos = jsonObj["Pos"].array_items();

	Vector3 trans;

	if (rPos.size() == 3)
	{
		trans.x = (float)rPos[0].number_value();
		trans.y = (float)rPos[1].number_value();
		trans.z = (float)rPos[2].number_value();

		mTrans.CreateTranslation(trans);
	}

	// 回転
	const std::vector<json11::Json>& rRot = jsonObj["Rot"].array_items();
	if (rRot.size() == 3)
	{
		mRotate.CreateRotationX((float)rRot[0].number_value() * Radians);
		mRotate.RotateY((float)rRot[1].number_value() * Radians);
		mRotate.RotateZ((float)rRot[2].number_value() * Radians);
	}

	// 拡大
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	if (rScale.size() == 3)
	{
		mScale.CreateScalling((float)rScale[0].number_value(), (float)rScale[1].number_value(),
			(float)rScale[2].number_value());
	}


	m_mWorld = mScale * mRotate * mTrans;

}

// 更新
void GameObject::Update()
{
	m_mPrev = m_mWorld;
}

// 描画
void GameObject::Draw()
{
	if (m_spModelComponent == nullptr) { return; }

	m_spModelComponent->Draw();
}

// ImGuiの更新
void GameObject::ImGuiUpdate()
{
	ImGui::InputText("Name", &m_name);

	// Tag
	if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::CheckboxFlags("Character", &m_tag, TAG_Character);
		ImGui::CheckboxFlags("Player", &m_tag, TAG_Player);
		ImGui::CheckboxFlags("StageObject", &m_tag, TAG_StageObject);
		ImGui::CheckboxFlags("AttackHit", &m_tag, TAG_AttackHit);
		ImGui::CheckboxFlags("Enemy", &m_tag, TAG_Enemy);
		ImGui::CheckboxFlags("DamegeObject", &m_tag, TAG_DamegeObject);

		// テキストへの保存
		if (ImGui::Button(u8"JSONテキストコピー"))
		{
			ImGui::SetClipboardText(Format("\"Tag\":%d", m_tag).c_str());	// \はJsonの中の " と同じ KdFormatはprintfみたいなもの
		}

		ImGui::TreePop();
	}

	// Transform
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 pos = m_mWorld.GetTranslation();
		Vector3 rot = m_mWorld.GetAngles() * Degrees;	// 表示したときにわかりやすいようにデグリー角に戻す
		//Vector3 scale = m_mWorld	// 表示したときにわかりやすいようにデグリー角に戻す

		bool isChange = false;

		isChange |= ImGui::DragFloat3("Position", &pos.x, 0.01f);	// 先頭のpos.xを渡すとx,y,zの全部渡せる?
		isChange |= ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
		//isChange |= ImGui::DragFloat3("Rotation", &, 0.1f);


		if (isChange)
		{
			// 計算するときはRadianに戻す
			rot *= Radians;

			Matrix mR;
			mR.RotateX(rot.x);
			mR.RotateY(rot.y);
			mR.RotateZ(rot.z);

			m_mWorld = mR;

			m_mWorld.SetTranslation(pos);
		}
		// テキストへの保存
		if (ImGui::Button(u8"JSONテキストコピー"))
		{
			std::string s = Format("\"Pos\":[%.1f,%.1f,%.1f],\n", pos.x, pos.y, pos.z);
			s += Format("\"Rot\":[%.1f,%.1f,%.1f],\n", rot.x, rot.y, rot.z);
			ImGui::SetClipboardText(s.c_str());
		}

		ImGui::TreePop();	// 忘れないこと!!!
	}
}

// 球による当たり判定(距離判定)
bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	// 当たったとする距離の計算(お互いの半径を足した値)
	float hitRange = rInfo.m_radius + m_colRadius;

	// 自分の座標ベクトル
	Vector3 myPos = m_mWorld.GetTranslation();

	// 二点間のベクトルを計算
	Vector3 betweenVec = rInfo.m_pos - myPos;

	// 二点間の距離を計算
	float distance = betweenVec.Length();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
}

//// レイによる当たり判定
//// レイ判定は結構重いらしい
//bool GameObject::HitCheckByRay(const RayInfo& rInfo, KdRayResult& rResult)
//{
//	// 無かったら早期リターン
//	if (!m_spModelComponent) { return false; }
//
//	// モデルコンポーネントはインスタンス化されているが、メッシュを読み込んでいなかった場合も変える
//	for (auto& node : m_spModelComponent->GetNodes())
//	{	
//		KdRayResult tmpResult;	//結果返送用
//
//		// レイ判定(本体からのずれ分も加味して計算)
//		return KdRayToMesh(rInfo.m_pos, rInfo.m_dir, rInfo.m_maxRange, *(node.m_spMesh),
//			node.m_localTransform * m_mWorld, tmpResult);
//
//		// より近い判定を優先する
//		if (tmpResult.m_distance < rResult.m_distance)
//		{
//			rResult = tmpResult;
//		}
//	}
//	return rResult.m_hit;
//}

// レイによる当たり判定
// レイ判定は結構重いらしい
bool GameObject::HitCheckByRay(const RayInfo& rInfo, RayResult& rResult)
{
	// 無かったら早期リターン
	if (!m_spModelComponent) { return false; }

	// モデルコンポーネントはインスタンス化されているが、メッシュを読み込んでいなかった場合も変える
	for (auto& node : m_spModelComponent->GetNodes())
	{
		if (!node.m_spMesh) { continue; }

		RayResult tmpResult; //結果返送用

		// レイ判定(本体からのずれ分も加味して計算)
		RayToMesh(rInfo.m_pos, rInfo.m_dir, rInfo.m_maxRange, *(node.m_spMesh),
			node.m_localTransform * m_mWorld, tmpResult);

		// より近い判定を優先する
		if (tmpResult.m_distance < rResult.m_distance)
		{
			rResult = tmpResult;
		}
	}
	return rResult.m_hit;
}

bool GameObject::HitCheckBySphereToMesh(const SphereInfo& rInfo, SphereResult& rResult)
{
	// 無かったら早期リターン
	if (!m_spModelComponent) { return false; }

	Vector3 pushedFromNodesPos = rInfo.m_pos;

	for (auto& node : m_spModelComponent->GetNodes())
	{
		if (!node.m_spMesh) { continue; }

		if (SphereToMesh(pushedFromNodesPos, rInfo.m_radius, *node.m_spMesh, node.m_localTransform * m_mWorld, pushedFromNodesPos))
		{
			rResult.m_hit = true;
		}

	}

	if (rResult.m_hit)
	{
		rResult.m_push = pushedFromNodesPos - rInfo.m_pos;
	}

	return rResult.m_hit;
}

// 開放
void GameObject::Release()
{
}

// クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "ActionGameProcess")
	{
		return std::make_shared<ActionGameProcess>();
	}

	if (name == "SceneManage")
	{
		return std::make_shared<SceneManeger>();
	}

	if (name == "SceneCameraManage")
	{
		return std::make_shared<SceneCameraManage>();
	}

	if (name == "SpawnManage")
	{
		return std::make_shared<SpawnManeger>();
	}

	if (name == "ShotGimmic")
	{
		return std::make_shared<ShotGimmic>();
	}

	if (name == "Lift")
	{
		return std::make_shared<Lift>();
	}

	if (name == "Target")
	{
		return std::make_shared<Target>();
	}

	if (name == "Item")
	{
		return std::make_shared<Item>();
	}

	if (name == "Boar")
	{
		return std::make_shared<Boar>();
	}

	if (name == "Alligator")
	{
		return std::make_shared<Alligator>();
	}

	if (name == "Eagle")
	{
		return std::make_shared<Eagle>();
	}

	if (name == "Bat")
	{
		return std::make_shared<Bat>();
	}

	if (name == "Human")
	{
		return std::make_shared<Human>();
	}

	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
	}


	// 文字列が既存のクラスに一致しなかった
	assert(0 && "存在しないGameObjectクラスです!");

	return nullptr;
}