#include "Sentry.h"
#include "../../Scene.h"
#include "../Gimmic/Bullet.h"
#include "../../../Component/ModelComponent.h"
#include "../../AnimationEffect.h"
#include"../../../../System/Debug/Debug.h"

void Sentry::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_spBullet = std::make_shared<Bullet>();

	Head = m_spModelComponent->FileNode("Head");

	
}

void Sentry::Update()
{
	if (!m_alive) { return; }

	switch (m_faze)
	{
	case Default:
		m_force.y = 0.1f;
		if (m_pos.y > 4.0f)
		{
			m_pos.y = 4.0f;
			m_faze = Action;
		}
		break;
	case Action:
		Snipe();
		break;
	case Attack:
		break;
	}


	
	m_pos += m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Sentry::Snipe()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_pos;
	info.m_radius = m_uniqueCol;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		Debug::GetInstance().AddDebugSphereLine(
			m_pos, m_uniqueCol, { 1.0f,0.0f,0.0f,1.0f }
		);

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Vector3 m_Player;

			m_Player = obj->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();

			//今のキャラクターの方向ベクトル
			Vector3 nowDir = m_mWorld.GetAxisZ();
			nowDir.Normalize();

			//キャラクターの今向いている方向の角度を求める（ラジアン角）
			float nowRadian = atan2(nowDir.x, nowDir.z);

			//移動方向へのベクトル方向の角度を求める（ラジアン角）
			float targetRadian = atan2(m_Player.x, m_Player.z);

			float rotateRadian = targetRadian - nowRadian;

			//atan2の結果-π～π（-180～180度）
			//180度の角度で数値の切れ目がある
			if (rotateRadian > M_PI)
			{
				rotateRadian -= 2 * float(M_PI);
			}
			if (rotateRadian < -M_PI)
			{
				rotateRadian += 2 * float(M_PI);
			}

			//一回のの回転角度をm_rotateAngle
			rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * Radians, m_rotateAngle * Radians);


			if (rotateRadian != 0)
			{
				m_rot.y += rotateRadian;
			}

			if (rotateRadian<0.01 && rotateRadian>-0.01)
			{
				if (m_FireLate < 0)
				{
					Shot();
					Explosion();
					m_FireLate = 60;
				}
				else
				{
					m_FireLate--;
				}
			}
		}
		else
		{
			m_wpTarget.reset();
		}
	}
}

void Sentry::Shot()
{
	auto bullet = std::make_shared<Bullet>();

	bullet->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/SentryBullet.json"));
	Matrix mat;
	mat.RotateY(m_rot.y);
	mat.SetTranslation(m_mWorld.GetTranslation()+Vector3(0,2.3,0));
	bullet->SetMatrix(mat);
	bullet->SetOwner(shared_from_this());
	Scene::GetInstance().AddObject(bullet);
}

void Sentry::Explosion()
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Explosion/Explosion02.png"), 5.0f, 5, 5, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat;
	Vector3 vec = m_mWorld.GetAxisZ()/2;
	hitMat.SetTranslation(vec+Vector3(0,1,0)+m_mWorld.GetTranslation());
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}
