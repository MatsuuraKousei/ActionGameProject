#include "Human.h"
#include"Enemy/Boar.h"
#include"Enemy/Bat.h"
#include"Enemy/Alligator.h"
#include"Weapon/Sword.h"
#include"Weapon/CrossBow.h"
#include"Weapon/Arrow.h"
#include "../Scene.h"
#include"../../../System/Debug/Debug.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"
#include "../../Component/ModelComponent.h"
#include"../AnimationEffect.h"
#include"../FixedTexture.h"
#include"ActionGameProcess.h"

const float Human::s_allowToStepHeight = 0.8f;
const float Human::s_landingHeight = 0.1f;

void Human::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	if (jsonObj["Pos"].is_array())
	{
		auto& p = jsonObj["Pos"].array_items();
		m_pos = Vector3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	if (m_spCameraComponent)
	{
		m_CameraTrans.x = 0.0f;
		m_CameraTrans.y = 2.5;
		m_CameraTrans.z = -5.0f;

		m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
		m_spCameraComponent->OffsetMatrix().RotateX(25.0f * Radians);
	}
	if ((GetTag() & TAG_Player) != 0)
	{
		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);

		//m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);
		m_spInputComponent = std::make_shared<ActionPlayerInputComponent>(*this);
	}

	if (m_spModelComponent)
	{
		ArmL = m_spModelComponent->FileNode("Arm.L");
		Body = m_spModelComponent->FileNode("Body");
		Head = m_spModelComponent->FileNode("Head");
	}


	SwordInit();
	CrossbowInit();

	IsSnipe() = false;

	MaxRange.x = 250;
	MaxRange.y = 100;
	MaxRange.z = 250;

	m_Hp = 4;

}

void Human::Update()
{
	if (Scene::GetInstance().debug) { return; }

	if (!m_alive) { return; }

	//Imguiコンポーネントの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	SwordUpdate();
	CrossbowUpdate();

	//移動前の座標を覚える
	m_prevPos = m_pos;


	if (m_spInputComponent->GetButton(Input::Buttons::Y) & m_spInputComponent->ENTER)
	{
		if (IsLockOn().lockon)
		{
			IsLockOn().lockon = false;
		}
		else
		{
			// 全ゲームオブジェクトのリストからミサイルが当たる対象を探す
			for (auto object : Scene::GetInstance().GetObjects())
			{
				// 自身は無視
				if (object.get() == this) { continue; }

				// それ以外は無視
				if (!(object->GetTag() & TAG_Enemy)) { continue; }

				m_wpTarget = object;
				IsLockOn().lockon = true;
			}
		}
	}

	if (!IsLockOn().lockon)
	{
		UpdateCamera();
	}
	else
	{
		Lockon();
	}

	//重力をキャラクターのYの移動力に加える
	if (m_gravityFlg)
	{
		m_force.y -= m_gravity;
	}

	if (m_alive)
	{
		//入力による移動の更新
		UpdateMove();

		//移動力をキャラクターの座標に足しこむ
		if (MaxRange.x + 200 >= m_pos.x && -MaxRange.x <= m_pos.x)
		{
			if (MaxRange.y >= m_pos.y && -MaxRange.y <= m_pos.y)
			{
				if (MaxRange.z >= m_pos.z && -MaxRange.z <= m_pos.z)
				{
					m_pos += m_force;
				}
			}
		}
	}

	//ワールド行列の合成する

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.Move(m_pos);



	//座標の更新を行った後に当たり判定
	UpdateCollision();

	if (!m_damegeStayFlg)
	{
		Damege();
		m_damegeStayFlg = true;
	}
	else
	{
		m_damegeStayTime--;
		if (m_damegeStayTime < 0)
		{
			m_damegeStayFlg = false;
			m_damegeStayTime = 20.0f;
		}
	}

	//カメラコンポーネントの更新
	if (m_spCameraComponent)
	{
		Matrix trans;
		trans.CreateTranslation(m_pos);
		m_spCameraComponent->SetCameraMatrix(trans);
	}

	if (m_alive)
	{

	}


	if (m_pos.y < 1)
	{
		m_Hp = 0;
	}
}

void Human::DrawEffect()
{
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	SHADER.m_effectShader.SetWorldMatrix(Matrix());
	SHADER.m_effectShader.WriteToCB();

	//m_propTrail.DrawStrip();

	// プロペラの軌跡の描画
	for (UINT i = 0; i < m_swordTrail.size(); i++)
	{
		m_swordTrail[i].DrawStrip();
	}

	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
}

void Human::UpdateMove()
{
	//カメラ方向に移動方向が依存するので、カメラが無かったら変える
	if (!m_spCameraComponent)return;

	//入力情報の取得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxiz(Input::Axes::L);
	//カメラの右方向＊レバー左右の入力＝キャラクター左右の移動方向
	Vector3 moveSide = m_spCameraComponent->GetCameraMatrix().GetAxisX() * inputMove.x;
	//カメラの前方向＊レバー前後の入力＝キャラクター前後の移動方向
	Vector3 moveForward = m_spCameraComponent->GetCameraMatrix().GetAxisZ() * inputMove.y;

	//上下方向への移動成分はカット
	moveForward.y = 0.0f;

	//移動ベクトルの計算
	moveVec = { moveSide + moveForward };

	//正規化（斜めに進まれないように）
	moveVec.Normalize();

	//キャラクターの回転処理
	UpdateRotate(moveVec);

	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;

	if (m_spInputComponent->GetButton(Input::A) & m_spInputComponent->ENTER && m_isGround)
	{
		m_force.y = 0.3f;
	}

}

void Human::SwordInit()
{
	m_spSword = std::make_shared<Sword>();

	m_spSword->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Sword.Json"));

	m_spSword->SetOwner(shared_from_this());

	m_spSword->SetMatrix(m_mWorld);

	m_fBodyInit = Body->m_localTransform.GetAngles().y;

	Scene::GetInstance().AddObject(m_spSword);
}

void Human::SwordUpdate()
{
	if (!m_bSword)
	{
		Matrix mat;

		Vector3 vec;

		vec.x = m_mWorld.GetTranslation().x + Body->m_localTransform.GetTranslation().x;
		vec.y = m_mWorld.GetTranslation().y + Body->m_localTransform.GetTranslation().y;
		vec.z = m_mWorld.GetTranslation().z + Body->m_localTransform.GetTranslation().z;


		mat.RotateX(4.72);
		mat.RotateY(m_rot.y + m_fswordInitAngle);
		mat.SetTranslation(vec);

		m_spSword->SetMatrix(mat);
	}

	if (IsSnipe()) { return; }

	if (m_spInputComponent->GetButton(Input::Buttons::L1) & m_spInputComponent->ENTER)
	{
		if (!m_bSword)
		{
			m_bSword = true;

		}
		else
		{
			m_bSword = false;
		}
	}

	if (m_bSword)
	{
		// 配列数のセット
		m_swordTrail.resize(1);
		// 画像のセット
		for (UINT i = 0; i < m_swordTrail.size(); i++)
		{
			m_swordTrail[i].SetTexture(ResFac.GetTexture("Data/Textures/2DTexture/Effect/sabelline.png"));
		}

		if (m_spInputComponent->GetButton(Input::Buttons::B) & m_spInputComponent->STAY)
		{
			m_fswordInitAngle -= 0.3f;
			Body->m_localTransform.RotateY(m_fswordInitAngle);

			SphereInfo sphereInfo;

			Vector3 vec;

			vec = m_spSword->GetMatrix().GetAxisY();

			vec.Normalize();

			sphereInfo.m_pos = vec + m_spSword->GetMatrix().GetTranslation();
			sphereInfo.m_radius = 0.4f;

			Vector3 push;
			//全員とレイ判定
			for (auto& obj : Scene::GetInstance().GetObjects())
			{
				//自分自身は無視
				if (obj.get() == this) { continue; }
				//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
				if (obj->GetTag() & TAG_Enemy)
				{
					SphereResult sphereResult;

					if (obj->HitCheckBySphereToMesh(sphereInfo, sphereResult))
					{
						obj->m_Hp--;
					}
				}
			}


			// エフェクト
			for (UINT i = 0; i < m_swordTrail.size(); i++)
			{
				Matrix swordOuterMat;
				// そこからY軸へ少しずらした位置(モデルのスケールが変わると適用しない)
				swordOuterMat.CreateTranslation(0.0f, 1.8f, 0.0f);
				swordOuterMat *= m_spSword->GetMatrix();

				// Strip描画するため２つで１ペア追加
				m_swordTrail[i].AddPoint(m_spSword->GetMatrix());
				m_swordTrail[i].AddPoint(swordOuterMat);

			}
		}
		if (m_spInputComponent->GetButton(Input::Buttons::X) & m_spInputComponent->STAY)
		{
			m_fswordInitAngle += 0.3f;
			Body->m_localTransform.RotateY(m_fswordInitAngle);


			SphereInfo sphereInfo;

			Vector3 vec;

			vec = m_spSword->GetMatrix().GetAxisY();

			vec.Normalize();

			sphereInfo.m_pos = vec + m_spSword->GetMatrix().GetTranslation();
			sphereInfo.m_radius = 0.4f;

			Vector3 push;
			//全員とレイ判定
			for (auto& obj : Scene::GetInstance().GetObjects())
			{
				//自分自身は無視
				if (obj.get() == this) { continue; }
				//ステージと当たり判定（背景オブジェクト以外に乗るときは変更の可能性あり）
				if (obj->GetTag() & TAG_Enemy)
				{
					SphereResult sphereResult;

					if (obj->HitCheckBySphereToMesh(sphereInfo, sphereResult))
					{
						obj->m_Hp--;
					}
				}
			}

			// エフェクト
			for (UINT i = 0; i < m_swordTrail.size(); i++)
			{
				Matrix swordOuterMat;
				// そこからY軸へ少しずらした位置(モデルのスケールが変わると適用しない)
				swordOuterMat.CreateTranslation(0.0f, 1.8f, 0.0f);
				swordOuterMat *= m_spSword->GetMatrix();

				// Strip描画するため２つで１ペア追加
				m_swordTrail[i].AddPoint(m_spSword->GetMatrix());
				m_swordTrail[i].AddPoint(swordOuterMat);

			}
		}

		Matrix mat;
		mat.RotateX(4.72);
		mat.RotateY(m_rot.y + m_fswordInitAngle);
		mat.SetTranslation(Body->m_localTransform.GetTranslation() + m_mWorld.GetTranslation());

		m_spSword->SetMatrix(mat);

		m_fEffectTime--;

		if (m_fEffectTime < 0)
		{
			m_swordTrail.clear();
			m_fEffectTime = 20;
		}

	}

}

void Human::CrossbowInit()
{
	m_spCrossbow = std::make_shared<CrossBow>();

	m_spCrossbow->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Crossbow.Json"));

	Scene::GetInstance().AddObject(m_spCrossbow);
}

void Human::CrossbowUpdate()
{
	// クロスボウアニメ
	if (IsSnipe())
	{
		if (ArmL)
		{
			Vector3 vec;
			Matrix mat;

			mat.RotateY(m_rot.y);
			vec.x += m_pos.x;
			vec.y += m_pos.y + 1.5;
			vec.z += m_pos.z;

			mat.CreateRotationX(m_spCameraComponent->GetCameraMatrix().GetAngles().x);
			mat.RotateY(m_spCameraComponent->GetCameraMatrix().GetAngles().y);
			mat.RotateZ(m_spCameraComponent->GetCameraMatrix().GetAngles().z);
			mat.SetTranslation(vec);

			m_spCrossbow->SetMatrix(mat);
		}
	}
	else
	{
		if (ArmL)
		{

			Matrix mat;
			Vector3 vec;

			vec.x += m_pos.x;
			vec.y += m_pos.y + 1.5;
			vec.z += m_pos.z;
			mat.RotateY(m_rot.y);
			mat.SetTranslation(vec);
			m_spCrossbow->SetMatrix(mat);
		}
	}

	if (m_bSword) { return; }
	// カメラ
	if (m_spInputComponent->GetButton(Input::Buttons::B) & m_spInputComponent->ENTER)
	{
		if (!IsSnipe())
		{
			//カメラを切り替える
			m_memory.x = m_CameraTrans.x;
			m_memory.y = m_CameraTrans.y;
			m_memory.z = m_CameraTrans.z;

			m_CameraTrans.x = 0.0f;
			m_CameraTrans.y = 2.5f;
			m_CameraTrans.z = -2.0f;
			m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
			m_spCameraComponent->OffsetMatrix().RotateX(-10.0f * Radians);
			m_spCameraComponent->OffsetMatrix().RotateY(m_rot.y);
			IsSnipe() = true;
		}
		else
		{
			//カメラを戻す

			m_CameraTrans = m_memory;

			m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
			m_spCameraComponent->OffsetMatrix().RotateY(m_rot.y);
			//m_spCameraComponent->OffsetMatrix().RotateX(25.0f * Radians);

			IsSnipe() = false;
		}
	}

	if (IsSnipe())
	{
		if (m_spInputComponent->GetButton(Input::Buttons::X) & m_spInputComponent->ENTER)
		{
			POINT center = { 1280 / 2,720 / 2 };

			Matrix mView = m_spCameraComponent->GetViewMatrix();
			Matrix mProj = m_spCameraComponent->GetProjMatrix();

			Vector3 NearPos = Scene::GetInstance().ConvertScreenToWorld(center.x, center.y, 0.0f, 1280, 720, mView, mProj);
			Vector3 FarPos = Scene::GetInstance().ConvertScreenToWorld(center.x, center.y, 1.0f, 1280, 720, mView, mProj);

			Vector3 Angle = FarPos - NearPos;

			Angle.Normalize();

			auto m_spArrow = std::make_shared<Arrow>();

			Vector3 Option;

			Option.x = 0 + Head->m_localTransform.GetTranslation().x;
			Option.y = 0.2 + Head->m_localTransform.GetTranslation().y;
			Option.z = 0 + Head->m_localTransform.GetTranslation().z;

			m_spArrow->m_pos = m_mWorld.GetTranslation() + Option;
			m_spArrow->m_rot = m_spCameraComponent->GetCameraMatrix().GetAngles();
			m_spArrow->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Arrow.Json"));

			m_spArrow->Position(Angle);

			m_spArrow->SetOwner(shared_from_this());
			Scene::GetInstance().AddObject(m_spArrow);

		}
	}


}

void Human::UpdateCamera()
{
	if (!m_spCameraComponent) { return; }

	const Math::Vector2& inputCamera = m_spInputComponent->GetAxiz(Input::Axes::R);

	float radX = inputCamera.x * m_camRotSpeed * Radians;
	float radY = inputCamera.y * m_camRotSpeed * Radians;

	if (radY > 1.0f)radY = 1.0f;
	else if (radY < -1.0f)radY = -1.0f;

	if (radX > 1.0f)radX = 1.0f;
	else if (radX < -1.0f)radX = -1.0f;

	m_spCameraComponent->OffsetMatrix().RotateY(radX);


	Vector3 CameraCompAxisX = m_spCameraComponent->OffsetMatrix().GetAxisX();

	m_spCameraComponent->OffsetMatrix().RotateAxis(CameraCompAxisX, radY);

}

void Human::Lockon()
{
	if (m_wpTarget.lock() == nullptr) { return; }

	auto Target = m_wpTarget.lock();

	IsLockOn().vec.x = Target->GetMatrix().GetTranslation().x;
	IsLockOn().vec.y = Target->GetMatrix().GetTranslation().y;

	Vector3 vec;

	Vector3 vtarget = Target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();
	vtarget.Normalize();

	Vector3 nowDir = m_spCameraComponent->OffsetMatrix().GetAxisZ();

	float nowRadianY = atan2(nowDir.x, nowDir.z);
	float targetRadianY = atan2(vtarget.x, vtarget.z);

	float nowRadianX = atan2(nowDir.y, nowDir.x);
	float targetRadianX = atan2(vtarget.y, vtarget.x);

	float dirY = targetRadianY - nowRadianY;
	float dirX = targetRadianX - nowRadianX;
	if (dirY > M_PI)
	{
		dirY -= 2 * float(M_PI);
	}
	if (dirY < -M_PI)
	{
		dirY += 2 * float(M_PI);
	}
	if (dirX > M_PI)
	{
		dirX -= 2 * float(M_PI);
	}
	if (dirX < -M_PI)
	{
		dirX += 2 * float(M_PI);
	}

	m_spCameraComponent->OffsetMatrix().RotateY(dirY);

	Vector3 CameraCompAxisX = m_spCameraComponent->OffsetMatrix().GetAxisX();

	m_spCameraComponent->OffsetMatrix().RotateAxis(CameraCompAxisX, 0);

	LockonPoint(Target->GetMatrix().GetTranslation());
}

//r_moveDir 移動方向
void Human::UpdateRotate(const Vector3& rMoveDir)
{
	if (rMoveDir.LengthSquared() == 0.0f) { return; }

	//今のキャラクターの方向ベクトル
	Vector3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	//キャラクターの今向いている方向の角度を求める（ラジアン角）
	float nowRadian = atan2(nowDir.x, nowDir.z);

	//移動方向へのベクトル方向の角度を求める（ラジアン角）
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

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

	m_rot.y += rotateRadian;
}

void Human::UpdateCollision()
{
	float distanceFromGround = FLT_MAX;

	//下方向への判定を行い、着地した
	if (CheckGround(distanceFromGround))
	{
		//地面の上にy座標を移動
		m_pos.y += s_allowToStepHeight - distanceFromGround;

		//地面があるので、y方向への移動力は０に
		m_force.y = 0.0f;
	}

	m_pos += CheckBump(m_pos);
}

bool Human::CheckGround(float& rDstDistance)
{
	//レイ判定情報
	RayInfo rayInfo;
	rayInfo.m_pos = m_pos;//キャラクターの位置を発射地点に

	//キャラの足元から例を発射すると地面と当たらないので少し持ち上げる（乗り越えられる高さ分だけ）
	rayInfo.m_pos.y += s_allowToStepHeight;
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
		m_isGround = (distanceFromGround < (s_allowToStepHeight + s_landingHeight));
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

Vector3 Human::CheckBump(Vector3 vec)
{
	SphereInfo sphereInfo;

	Vector3 pos = Vector3(0, 0, 0);

	sphereInfo.m_pos = vec;
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
				push += sphereResult.m_push;
			}
		}
	}
	pos += push;
	return pos;
}

void Human::Damege()
{
	// 球情報の作成
	SphereInfo info;
	info.m_pos = m_pos;
	info.m_radius = 1;
	Debug::GetInstance().AddDebugSphereLine(
		m_mWorld.GetTranslation(), info.m_radius, { 1.0f,0.0f,0.0f,1.0f }
	);
	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		// キャラクターと当たり判定をするのでそれ以外は無視
		if (obj->GetTag() & TAG_Enemy)
		{
			// 当たり判定
			if (obj->HitCheckBySphere(info))
			{
				Explosion(Body->m_localTransform.GetTranslation() + m_pos);
				//m_Hp--;
			}
		}


		if (obj->GetTag() & TAG_DamegeObject)
		{
			SphereResult sphereResult;
			if (obj->HitCheckBySphereToMesh(info, sphereResult))
			{
				Explosion(Body->m_localTransform.GetTranslation() + m_pos);
				Vector3 KnockBack = -m_mWorld.GetAxisZ();
				KnockBack.Normalize();
				m_force = KnockBack;
				m_force.y += 0.5;
				m_Hp--;
			}
		}
	}

}

void Human::LockonPoint(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<FixedTexture> effect = std::make_shared<FixedTexture>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetInfo(ResFac.GetTexture("Data/Textures/2DTexture/Battle/CenterPoint.png"), 0.3f, (float)(rand() % 360));

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}

// 爆発
void Human::Explosion(const Vector3& hitPos)
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(ResFac.GetTexture("Data/Textures/2DTexture/Effect/Attack.png"), 5.0f, 3, 3, (float)(rand() % 360), 0.9f);

	// 場所を自分の位置に合わせる
	Matrix hitMat = m_mWorld;
	hitMat.SetTranslation(hitPos);
	effect->SetMatrix(hitMat);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}


