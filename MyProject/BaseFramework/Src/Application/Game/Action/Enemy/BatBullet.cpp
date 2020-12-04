#include "BatBullet.h"
#include "../../../main.h"
#include"../../Scene.h"

void BatBullet::Deserialize(const json11::Json& jsonObj)
{
	// 無かったら早期リターン
	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	// 生きる時間(最大Fpsに依存?)
	m_lifeSpan = APP.m_maxFps * 2;
}

void BatBullet::Update()
{
	if (Scene::GetInstance().debug) { return; }
	// 死んでいる場合早期リターン
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		// 消すかどうかはSceneが見る
		Destroy();
	}


	auto target = m_wpTarget.lock();

	if (target)
	{
		if (!m_targetting)
		{
			// 自身からターゲットへのベクトル
			Vector3 vTarget = target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();


			// 単位ベクトル化：自身からターゲットへ向かうながさ１のベクトル
			vTarget.Normalize();

			// 自分のZ方向(前方向)
			Vector3 vZ = m_mWorld.GetAxisZ();

			// 拡大率が入っていると計算がおかしくなるため単位ベクトル化
			vZ.Normalize();

			// ※※※※※回転軸作成(この軸で回転する)※※※※※
			Vector3 vRotAxis = Vector3::Cross(vZ, vTarget);

			// 0ベクトルなら回転しない
			if (vRotAxis.LengthSquared() != 0)
			{
				// 自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
				float d = Vector3::Dot(vZ, vTarget);

				// 誤差で-1～1以上になる可能性大なので、クランプする(std::clampでも可)
				if (d > 1.0f)d = 1.0f;
				else if (d < -1.0f)d = -1.0f;

				// 自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度(radian)
				float radian = acos(d);	// ラジアン角に変換

				// 角度制限　１フレームにつき最大で１度以上回転しない
				if (radian > 1.0f * Radians)
				{
					radian = 1.0f * Radians;
				}

				// ※※※※※radian （ここまでで回転角度が求まった）※※※※※


				Matrix mRot;
				mRot.CreateRotationAxis(vRotAxis, radian);

				//m_mWorld = mRot * m_mWorld;
				// 修正
				auto pos = m_mWorld.GetTranslation();
				m_mWorld.SetTranslation({ 0,0,0 });
				m_mWorld *= mRot;
				m_mWorld.SetTranslation(pos);
			}
		}

		Vector3 move = m_mWorld.GetAxisZ();
		move.Normalize();


		move *= (float)m_speed;

		m_mWorld.Move(move);

		// 球情報の作成
		SphereInfo info;
		info.m_pos = m_mWorld.GetTranslation();
		info.m_radius = 0.01;
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
				Destroy();
			}
		}

		m_time--;
		if (m_time < 0)
		{
			m_targetting = true;
		}
	}
}