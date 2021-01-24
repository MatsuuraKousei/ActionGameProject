#include "Arrow.h"
#include"../../../main.h"
#include"../../Scene.h"
#include"../../AnimationEffect.h"
#include "../Manage/ScoreManager.h"

void Arrow::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// 生きる時間(最大Fpsに依存?)
	m_lifeSpan = APP.m_maxFps * 2;

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Arrow::Update()
{
	// 死んでいる場合早期リターン
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		// 消すかどうかはSceneが見る
		Destroy();
	}

	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();


	if (!m_stop)
	{
		m_Axiz.y -= m_gravity;
		m_mWorld.Move(m_Axiz);
	}

	Collision();
}

void Arrow::Collision()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 0.05;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身は無視
		if (obj.get() == this) { continue; }

		// 発射した主人も無視
		if (obj.get() == m_wpOwner.lock().get()) { continue; }

		// 当たったかどうか判定
		bool Hit = false;

		// TAG_Characterとは球判定を行う
		if ((obj->GetTag() & TAG_Enemy))
		{
			

			if (obj->HitCheckBySphere(info))
			{
				obj->m_Hp--;
				ScoreManager::GetInstance().AllEnemies++;
				Explosion3(m_mWorld.GetTranslation());
				Destroy();
			}
		}

		if ((obj->GetTag() & TAG_StageObject))
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				m_stop = true;
				Explosion(m_mWorld.GetTranslation());
				Destroy();
			}
		}

		if ((obj->GetTag() & TAG_AttackHit))
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphere(info))
			{
				if (obj->m_Hp > 1)
				{
					obj->m_Hp--;
				}
				Explosion2(m_mWorld.GetTranslation());
				Destroy();
			}
		}
	}
}

void Arrow::CheckBump()
{
	SphereInfo sphereInfo;

	Vector3 pos = Vector3(0, 0, 0);

	sphereInfo.m_pos = m_pos;
	sphereInfo.m_pos.z += 1.0f;
	sphereInfo.m_pos.y += 0.8f;
	sphereInfo.m_radius = 0.4f;

	Vector3 push;
	//全員とレイ判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
		if (obj->GetTag() & TAG_StageObject)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(sphereInfo, sphereResult))
			{
				m_stop = true;
			}
		}
	}
}

void Arrow::Position(Vector3 Angle)
{
	m_Axiz = Angle;
}

void Arrow::Explosion(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Effect/Arrow.png"), 5.0f, 5, 2, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}

void Arrow::Explosion2(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Explosion/Explosion03.png"), 5.0f, 5, 3, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}

void Arrow::Explosion3(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Explosion/Explosion04.png"), 5.0f, 5, 2, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}
