#include "Monkey.h"
#include "../Manage/ScoreManager.h"
#include "../../Scene.h"
#include "System/Debug/Debug.h"

void Monkey::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	m_Hp = 3;

	m_colRadius = 1;

	m_moveStoper = false;

	m_spShield = std::make_shared<GameObject>();
	m_spShield->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Shield.json"));
	m_spShield->m_Hp = 50;
	m_spShield->m_colRadius = 2.0f;
	Scene::GetInstance().AddObject(m_spShield);

}

void Monkey::Update()
{
	if (m_Hp < 0)
	{
		ScoreManager::GetInstance().AllEnemies++;
	}

	if (!m_alive)
	{
		m_spShield->Destroy();
	}

	if (!m_alive) { return; }

	if (m_bShield)
	{
		Matrix mat;
		mat.RotateY(m_rot.y + 4.7);
		mat.SetTranslation(m_mWorld.GetAxisZ() + m_mWorld.GetTranslation() + Vector3(0, 1.5, 0));
		m_spShield->SetMatrix(mat);
	}
	Debug::GetInstance().AddDebugSphereLine(m_spShield->GetMatrix().GetTranslation(), 2.0);



	switch (m_faze)
	{
	case Default:
		m_force.y = 0.1f;
		if (m_pos.y > 4.7)
		{
			m_pos.y = 4.7;
			m_faze = Action;
		}
		break;
	case Action:

		//Collision();
		MoveStop();
		if (!m_moveStoper)
		{
			Move();
		}
		break;
	case Attack:

		//Collision();
		break;
	}



	m_pos += m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Monkey::Move()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_uniqueCol;
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

			if (rotateRadian<0.02 && rotateRadian>-0.02)
			{
				Vector3 move = m_mWorld.GetAxisZ();
				move.Normalize();

				move *= (float)m_speed;

				m_force = m_force + move;
			}
		}

	}
}

#define Stopcol 1.5

void Monkey::MoveStop()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = Stopcol;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			m_moveStoper = true;
		}
		else
		{
			m_moveStoper = false;
		}
	}
}

void Monkey::Collision()
{
	float distanceFromGround = FLT_MAX;



	SphereInfo sphereInfo;

	sphereInfo.m_pos = m_pos;
	sphereInfo.m_radius = 1.0f;

	Vector3 push;
	//全員とレイ判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
		if (obj->GetTag() & TAG_KnockBack)
		{
			SphereResult sphereResult;

			if (obj->HitCheckBySphereToMesh(sphereInfo, sphereResult))
			{
				m_bShield = false;
			}


		}
	}

}

bool Monkey::Ground(float rDstDistance)
{
	//レイ判定情報
	RayInfo rayInfo;
	rayInfo.m_pos = m_pos;//キャラクターの位置を発射地点に

	//キャラの足元から例を発射すると地面と当たらないので少し持ち上げる（乗り越えられる高さ分だけ）
	rayInfo.m_pos.y += 0.8;
	//落下中かもしれないので、1フレーム前の座標分も持ち上げる
	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;
	//地面方向へのレイ
	rayInfo.m_dir = { 0.0f,-1.0f,0.0f };

	//レイの結果格納用
	rayInfo.m_maxRange = FLT_MAX;
	RayResult finalRayResult;

	std::shared_ptr<GameObject> hitObj = nullptr;

	//全員とレイ判定
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }
		//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
		if (obj->GetTag() & TAG_StageObject)
		{
			RayResult rayResult;

			if (obj->HitCheckByRay(rayInfo, rayResult))
			{
				//もっとも当たったところまでの距離が短いものを保持する
				if (rayResult.m_distance < finalRayResult.m_distance)
				{
					finalRayResult = rayResult;

					hitObj = obj;
				}
			}
		}
	}
	//補正分の長さを結果に反映＆着地判定
	float distanceFromGround = FLT_MAX;
	//足元にステージオブジェクトがあった
	if (finalRayResult.m_hit)
	{
		//地面との距離を算出
		distanceFromGround = finalRayResult.m_distance - (m_prevPos.y - m_pos.y);
	}
	//上方向に力がかかっていた場合
	if (m_force.y > 0.0f)
	{
		//着地禁止
		m_isGround = false;
	}
	else
	{
		//地面からの距離が（歩いて乗り越えれる高さ＋地面から足が離れていても着地判定する高さ）未満であれば着地とみなす
		m_isGround = (distanceFromGround < (0.8 + 0.1));
	}
	//地面との距離を格納
	rDstDistance = distanceFromGround;

	//動くものの上に着地した時の判定（地面判定がないと連れていかれる）
	if (hitObj && m_isGround)
	{
		//相手の一回動いた分を取得
		auto mOneMove = hitObj->GetOneMove();
		auto vOneMove = mOneMove.GetTranslation();

		//相手の動いた分を自分の移動に含める
		m_pos = m_pos + vOneMove;
	}

	//着地したかを返す
	return m_isGround;
}
