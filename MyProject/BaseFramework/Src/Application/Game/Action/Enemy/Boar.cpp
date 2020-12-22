#include "Boar.h"
#include"../Human.h"
#include "../../Scene.h"
#include"../../FixedTexture.h"
#include"../Manage/SpawnManager.h"

Boar::Boar()
{
	m_Hp = 3;
	
}

void Boar::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);
	m_Hp = 3;


}

void Boar::Update()
{
	if (!m_alive) { return; }

	m_prevPos = m_mWorld.GetTranslation();

	// 行動
	switch (m_faze)
	{
	case Default:
		m_force.y = 0.1f;
		if (m_pos.y > 4)
		{
			m_pos.y = 4;
			m_faze = Action;
		}
		break;
	case Action:
		Snipe();
		Hp();
		break;
	case Attack:
		Move();
		Hp();
		break;
	}

	if (EndlessMode)
	{
		if (m_Hp < 0)
		{
			IsEndless() = false;
		}
	}

	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);



	UpdateCollision();
}

void Boar::Move()
{
	if (!StopMove) { return; }

	// 変える前に座標を保存
	m_prevPos = m_mWorld.GetTranslation();

	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();


	
	if (AttackCounter > 0)
	{
		Vector3 move = m_mWorld.GetAxisZ();
		move.Normalize();

		move *= (float)m_speed;

		m_force = m_force + move;

		AttackCounter--;

	}
	else
	{
		StopMove = true;
		if (WaitCounter < 0)
		{
			// 球情報の作成
			SphereInfo info;
			info.m_pos = m_mWorld.GetTranslation();
			info.m_radius = m_uniqueCol-5;
			for (auto& obj : Scene::GetInstance().GetObjects())
			{
				// 自分自身を無視
				if (obj.get() == this) { continue; }

				// キャラクターと当たり判定をするのでそれ以外は無視
				if (!(obj->GetTag() & TAG_Player)) { continue; }

				// 当たり判定
				if (obj->HitCheckBySphere(info))
				{
					if (!m_falseFlg)
					{
						m_faze = Action;
					}
				}
				else
				{
					m_falseFlg = true;
				}
			}
			if (m_falseFlg)
			{
				m_force.y = -0.1f;
				if (m_pos.y < -2)
				{
					m_alive = false;
				}
			}
		}
		WaitCounter--;
	}
}

void Boar::Snipe()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_uniqueCol+5;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			/*Scene::GetInstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), m_uniqueCol, { 1.0f,0.0f,0.0f,1.0f }
			);*/

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
				m_faze = Attack;
			}
		}

	}
}

void Boar::UpdateCollision()
{
	
}

void Boar::VectorMove(Matrix mat)
{

	Vector3 rMoveDir = mat.GetTranslation();

	// 単位ベクトル化：自身からターゲットへ向かうながさ１のベクトル
	rMoveDir.Normalize();

	// 自分のZ方向(前方向)
	Vector3 vZ = m_mWorld.GetAxisZ();
	// 拡大率が入っていると計算がおかしくなるため単位ベクトル化
	vZ.Normalize();

	// キャラの今向いている方向の角度を求める
	float nowRadian = atan2(vZ.x, vZ.z);

	//移動する方向へのベクトルの角度を求める（ラジアン角）
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

	// 今向いてる方向と目的の方向との差を求める
	float rotateRadian = targetRadian - nowRadian;

	// -π ～ π（-180度 ～ 180度）
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	// 一回の回転角度をm_rotateAngle以内に収める（クランプ）
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * Radians, m_rotateAngle * Radians);

	m_rot.y += rotateRadian;
}

void Boar::Hp()
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
