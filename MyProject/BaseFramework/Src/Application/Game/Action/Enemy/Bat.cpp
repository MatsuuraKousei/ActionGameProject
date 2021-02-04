#include "Bat.h"
#include "../../Scene.h"
#include"../../../../System/Debug/Debug.h"
#include"../Manage/SpawnManager.h"
#include"../Manage/ScoreManager.h"
#include"BatBullet.h"
#include"../Gimmic/Bullet.h"
#include"../../AnimationEffect.h"
#include"../../FixedTexture.h"
#include "../../../Component/ModelComponent.h"

Bat::Bat()
{
	m_Hp = 3;
	
}

void Bat::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_Hp = 3;

	
}

void Bat::Update()
{
	if (Scene::GetInstance().debug) { return; }

	if (EndlessMode)
	{
		if (!m_alive)
		{
			IsEndless() = false;
		}
	}

	if (m_Hp <= 0)
	{
		Explosion(m_mWorld.GetTranslation());
		ScoreManager::GetInstance().AllEnemies++;
	}

	if (!m_alive) { return; }

	// 行動
	switch (m_faze)
	{
	case Default:

		// 羽アニメ
		Move();
		if (m_pos.y > 7)
		{
			m_pos.y = 7;

			// 召喚アニメーション
			Appearance();

			m_faze = Action;
		}
		else
		{
			m_force.y = 0.2f;
			m_rot.y += 5;
		}
		break;
	case Action:
		//HP();
		Shot();
		break;
	case Attack:
		break;
	}

	if (roop < 0)
	{
		roop = 0;
	}


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

			m_moveFlg = false;

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


			StopMove = true;

			if (rotateRadian != 0)
			{
				m_rot.y += rotateRadian;
			}

			if (rotateRadian<0.02 && rotateRadian>-0.02)
			{
				AttackCounter = 60;
				m_wpTarget = obj;
				m_faze = Action;
			}
		}
		else
		{
			m_wpTarget.reset();
		}
	}

	


	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Bat::Appearance()
{
	if (m_Animation) { return; }
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Enemy/Effect/BatEffect.png"), 5.0f, 5, 3, (float)(rand() % 360), 1.1f);
	// 場所を自分の位置に合わせる

	Matrix m;
	Vector3 v;
	v.x = m_pos.x;
	v.y = m_pos.y + 0.5;
	v.z = m_pos.z + 0.1;

	m.CreateTranslation(v);

	effect->SetMatrix(m);

	// リストに追加
	Scene::GetInstance().AddObject(effect);

	m_Animation = true;
}

void Bat::Move()
{
	Model::Node* WingR = m_spModelComponent->FileNode("WingR");
	Model::Node* WingL = m_spModelComponent->FileNode("WingL");

	if (m_faze == Default)
	{
		Vector3 vec;
		vec.x = WingR->m_localTransform.GetTranslation().x;
		vec.y = WingR->m_localTransform.GetTranslation().y;
		vec.z = WingR->m_localTransform.GetTranslation().z;

		if (!m_WingFlg[0])
		{
			WingR->m_localTransform.RotateZ(-1.2);
			m_WingFlg[0] = true;
		}
		WingR->m_localTransform.SetTranslation(vec);

		Vector3 vec2;
		vec2.x = WingL->m_localTransform.GetTranslation().x;
		vec2.y = WingL->m_localTransform.GetTranslation().y;
		vec2.z = WingL->m_localTransform.GetTranslation().z;

		if (!m_WingFlg[1])
		{
			WingL->m_localTransform.RotateZ(1.2);
			m_WingFlg[1] = true;
		}
		WingL->m_localTransform.SetTranslation(vec);

	}

	if (m_pos.y > 3)
	{
		Vector3 vec;
		vec.x = WingR->m_localTransform.GetTranslation().x;
		vec.y = WingR->m_localTransform.GetTranslation().y;
		vec.z = WingR->m_localTransform.GetTranslation().z;

		if (m_WingFlg[0])
		{
			WingR->m_localTransform.RotateZ(1.2);
			m_WingFlg[0] = false;
		}
		WingR->m_localTransform.SetTranslation(vec);

		Vector3 vec2;
		vec2.x = WingL->m_localTransform.GetTranslation().x;
		vec2.y = WingL->m_localTransform.GetTranslation().y;
		vec2.z = WingL->m_localTransform.GetTranslation().z;

		if (m_WingFlg[1])
		{
			WingL->m_localTransform.RotateZ(-1.2);
			m_WingFlg[1] = false;
		}
		WingL->m_localTransform.SetTranslation(vec);

	}


}

void Bat::HP()
{
	if (m_Hp >= 1)
	{
		// 1///////////////////////////////////////////////////////////////////////////////////////////
		// アニメーションエフェクトをインスタンス化
		std::shared_ptr<FixedTexture> one = std::make_shared<FixedTexture>();

		// 爆発のテクスチャとアニメーション情報を渡す
		one->SetInfo(ResFac.GetTexture("Data/Textures/2DTexture/Enemy/NormalHP/HP.png"), 0.5f, 0);
		// 場所を自分の位置に合わせる

		Matrix m1;
		Vector3 v1;

		//v1 = m_mWorld.GetAxisX();
		v1.x += m_pos.x;
		v1.y += m_pos.y - 0.5;
		v1.z += m_pos.z;

		m1.CreateTranslation(v1);

		one->SetMatrix(m1);

		// リストに追加
		Scene::GetInstance().AddObject(one);

		if (m_Hp >= 2)
		{
			// 2///////////////////////////////////////////////////////////////////////////////////////////
			// アニメーションエフェクトをインスタンス化
			std::shared_ptr<FixedTexture> two = std::make_shared<FixedTexture>();

			// 爆発のテクスチャとアニメーション情報を渡す
			two->SetInfo(ResFac.GetTexture("Data/Textures/2DTexture/Enemy/NormalHP/HP.png"), 0.5f, 0);
			// 場所を自分の位置に合わせる

			Matrix m2;
			Vector3 v2;

			//v2 = -m_mWorld.GetAxisX();
			v2.x += m_pos.x + 0.7;
			v2.y += m_pos.y - 0.5;
			v2.z += m_pos.z;

			m2.CreateTranslation(v2);

			two->SetMatrix(m2);

			// リストに追加
			Scene::GetInstance().AddObject(two);

			if (m_Hp >= 3)
			{
				// 3///////////////////////////////////////////////////////////////////////////////////////////
				// アニメーションエフェクトをインスタンス化
				std::shared_ptr<FixedTexture> three = std::make_shared<FixedTexture>();

				// 爆発のテクスチャとアニメーション情報を渡す
				three->SetInfo(ResFac.GetTexture("Data/Textures/2DTexture/Enemy/NormalHP/HP.png"), 0.5f, 0);
				// 場所を自分の位置に合わせる

				Matrix m3;
				Vector3 v3;

				//v3 = m_mWorld.GetAxisX();
				v3.x += m_pos.x - 0.7;
				v3.y += m_pos.y - 0.5;
				v3.z += m_pos.z;

				m3.CreateTranslation(v3);

				three->SetMatrix(m3);

				// リストに追加
				Scene::GetInstance().AddObject(three);
			}
		}
	}
}

void Bat::Shot()
{
	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();

	if (!target) { return; }

	m_AttackState--;
	if (m_AttackState < 0)
	{
		auto spBBullet = std::make_shared<BatBullet>();
		if (spBBullet)
		{
			spBBullet->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/BatBullet.json"));
			spBBullet->SetMatrix(m_mWorld);
			spBBullet->SetOwner(shared_from_this());
			spBBullet->SetTarget(target);
			Scene::GetInstance().AddObject(spBBullet);
		}
		m_AttackState = 80;
	}
}

void Bat::Explosion(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Explosion/Explosion05.png"), 10.0f, 5, 3, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}
