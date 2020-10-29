#include "Item.h"
#include"../Scene.h"

void Item::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_Rotate = 0.0f;
	m_Trans = { 0,0,0 };
}

void Item::Update()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = 1;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{

		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			m_alive = false;
		}
	}
}
