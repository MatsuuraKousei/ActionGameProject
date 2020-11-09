#include "Eagle.h"
#include "../../Scene.h"
#include"../Manage/SpawnManager.h"

void Eagle::Update()
{
	if (!m_alive) { return; }

	// 行動
	switch (m_faze)
	{
	case Default:
		if (m_moveFlg)
		{
			Move();
			m_rot.y += 0.04;
		}
		break;
	case Action:
		Targetting();
		break;
	case Attack:
		break;
	}

	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_target;
	info.m_radius = m_uniqueCol;
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (!(obj->GetTag() & TAG_Player)) { continue; }

		Scene::GetInstance().AddDebugSphereLine(
			m_target, m_uniqueCol, { 1.0f,0.0f,0.0f,1.0f }
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

			if (rotateRadian<0.01 && rotateRadian>-0.01)
			{
				AttackCounter = 60;
				m_wpTarget = obj;
				m_faze = Action;
			}
		}
	}


	m_pos = m_pos + m_force;
	m_force = { 0,0,0 };

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.SetTranslation(m_pos);
}

void Eagle::Move()
{
	//m_force.x = 0.2 * cos(Radians * m_moveAngle) + sm.GetMatrix().GetTranslation().x;
	//m_force.z = 0.2 * sin(Radians * m_moveAngle) + sm.GetMatrix().GetTranslation().z;

	/*m_moveAngle++;

	if (m_moveAngle > 360)
	{
		m_moveAngle = 0;
	}*/



	Vector3 moveVec = m_mWorld.GetAxisZ();
	moveVec.Normalize();

	m_force.x = moveVec.x * 0.3f;
	m_force.z = moveVec.z * 0.3f;


}

void Eagle::Targetting()
{
	Vector3 moveVec = m_mWorld.GetAxisZ();
	moveVec.Normalize();

	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();

	if (target)
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

			// 誤差で-1～1以上になる可能性大なので、クランプする(std;;clampでも可)
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

	m_force += move;
}
