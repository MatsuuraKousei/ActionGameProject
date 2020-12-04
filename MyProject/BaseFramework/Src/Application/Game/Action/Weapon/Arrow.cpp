#include "Arrow.h"
#include"../../../main.h"
#include"../../Scene.h"

void Arrow::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// 生きる時間(最大Fpsに依存?)
	m_lifeSpan = APP.m_maxFps * 1;

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

	
	m_mWorld.Move(m_Axiz);

	Collision();
}

void Arrow::Collision()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 0.1;

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
				obj->Destroy();
				Destroy();
			}
		}
	}
}

void Arrow::Position(Vector3 Angle)
{
	m_Axiz = Angle;
}
