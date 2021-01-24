#include "Bullet.h"
#include"../../Scene.h"
#include "../../AnimationEffect.h"
#include "../../../main.h"

void Bullet::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// 生きる時間(最大Fpsに依存?)
	m_lifeSpan = APP.m_maxFps*2;
}

void Bullet::Update()
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

	Vector3 move = m_mWorld.GetAxisZ() + m_GimmicAngle;
	move.Normalize();

	move *= (float)m_speed;

	m_mWorld.Move(move);

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 0.2;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			obj->m_Hp--;
			Explosion();
			Destroy();
		}
	}

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
		if (obj->GetTag() & TAG_StageObject)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				Explosion();
				Destroy();
			}
		}
		if (obj->GetTag() & TAG_ActiveObject)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				Explosion();
				Destroy();
			}
		}
	}
}

void Bullet::GetGimmicAngle(Vector3 vec)
{
	m_GimmicAngle = vec;
}

void Bullet::Explosion()
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Explosion/Explosion01.png"), 5.0f, 5, 3, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	effect->SetMatrix(m_mWorld);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}
