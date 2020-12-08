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
		m_CameraTrans.y = 2.5f;
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
		//再生するアニメーションデータを保持しとく
		m_spAnimation = m_spModelComponent->GetAnimation("Stand");

		ArmL = m_spModelComponent->FileNode("Arm.L");
		Body = m_spModelComponent->FileNode("Body");
	}

	SwordInit();
	CrossbowInit();

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

	// 簡易移動制限
	if (m_pos.x >= MaxRange.x + 10)
	{
		m_pos.x = MaxRange.x + 10;
	}
	if (m_pos.x <= -MaxRange.x)
	{
		m_pos.x = -MaxRange.x;
	}
	if (m_pos.y >= MaxRange.y)
	{
		m_pos.y = MaxRange.y;
	}
	if (m_pos.y <= -MaxRange.y)
	{
		m_pos.x = -MaxRange.y;
	}
	if (m_pos.z >= MaxRange.z)
	{
		m_pos.z = MaxRange.z;
	}
	if (m_pos.z <= -MaxRange.z + 160)
	{
		m_pos.z = -MaxRange.z + 160;
	}


	//ワールド行列の合成する
	if (!IsSnipe())
	{
		m_mWorld.CreateRotationX(m_rot.x);
		m_mWorld.RotateY(m_rot.y);
		m_mWorld.RotateZ(m_rot.z);
		m_mWorld.Move(m_pos);
	}
	else
	{
		Vector3 vec;
		vec = m_spCameraComponent->GetCameraMatrix().GetAngles();

		m_mWorld.CreateRotationY(vec.y);
		m_mWorld.Move(m_pos);
	}

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
		//アニメーションの更新
		if (m_spAnimation && m_spModelComponent)
		{
			auto& rModelNode = m_spModelComponent->GetChangebleNodes();

			//全てのアニメーションノード（モデルの行列を補完する情報）の行列補間を実行する
			for (auto& rAnimNode : m_spAnimation->m_nodes)
			{
				//対応するモデルノードのインデックス
				UINT idx = rAnimNode.m_nodeOffset;

				//対応するモデルノードの行列補完を実行

				//クォーターニオンによる回転補間
				Matrix rotate;
				Quaternion resultQuat;
				if (rAnimNode.InterpolateRotations(resultQuat, m_animationTime))
				{
					rotate.CreateFromQuaternion(resultQuat);
				}

				//ベクターによる座標補間
				Matrix trans;
				Vector3 resultVec;
				if (rAnimNode.InterpolateTranslations(resultVec, m_animationTime))
				{
					trans.CreateTranslation(resultVec);
				}

				rModelNode[idx].m_localTransform = rotate * trans;
			}

			//アニメーションのフレームを１フレーム進める
			m_animationTime += 1.0f;

			//アニメーションデータの最後のフレームを超えたらアニメーションの最初に戻る（ループさせる
			if (m_animationTime >= m_spAnimation->m_maxLength) { m_animationTime = 0.0f; }
		}
	}


	if (m_pos.y < -2.5)
	{
		m_Hp--;
		m_force.y = 0.3f;
	}
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

	//ダッシュ
	if (m_spInputComponent->GetButton(Input::L1) & m_spInputComponent->STAY && m_isGround)
	{
		m_moveSpeed = 0.35f;
	}
	else
	{
		m_moveSpeed = 0.2f;
	}

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

	//m_spSword->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Sword.Json"));

	m_spSword->SetOwner(shared_from_this());

	Scene::GetInstance().AddObject(m_spSword);
}

void Human::SwordUpdate()
{
	Vector3 vArmRHand;

	Model::Node* ArmR = m_spModelComponent->FileNode("Arm.R");
	Model::Node* HandR = m_spModelComponent->FileNode("Hand.R");

	if (ArmR)
	{
		Vector3 pos;
		pos.x = ArmR->m_localTransform.GetTranslation().x;
		pos.y = ArmR->m_localTransform.GetTranslation().y;
		pos.z = ArmR->m_localTransform.GetTranslation().z;

		//ArmR->m_localTransform.RotateX(0.01f);

		ArmR->m_localTransform.SetTranslation(pos);

		Matrix armmat;
		armmat.CreateRotationX(ArmR->m_localTransform.GetAxisZ().x);

		HandR->m_localTransform.SetTranslation(armmat.GetTranslation());

		Vector3 pos2;
		pos2.x = HandR->m_localTransform.GetTranslation().x;
		pos2.y = HandR->m_localTransform.GetTranslation().y;
		pos2.z = HandR->m_localTransform.GetTranslation().z;


		Matrix mat;
		vArmRHand.x += m_pos.x;
		vArmRHand.y += m_pos.y;
		vArmRHand.z += m_pos.z;

		vArmRHand += pos2;
		mat.SetTranslation(vArmRHand);
		m_spSword->SetMatrix(mat);
	}
	if (m_Hp < 0)
	{
		m_spSword->Destroy();
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

	if (m_spInputComponent->GetButton(Input::Buttons::B) & m_spInputComponent->STAY)
	{
		if (!IsSnipe())
		{
			//カメラを切り替える
			m_memory.x = m_CameraTrans.x;
			m_memory.y = m_CameraTrans.y;
			m_memory.z = m_CameraTrans.z;

			m_CameraTrans.x = 1.0f;
			m_CameraTrans.y = 1.3f;
			m_CameraTrans.z = -2.0f;
			m_rot.y = m_spCameraComponent->GetCameraMatrix().GetAngles().y;
			m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
			m_spCameraComponent->OffsetMatrix().RotateX(-10.0f * Radians);
			m_spCameraComponent->OffsetMatrix().RotateY(m_rot.y);
		}


		IsSnipe() = true;
	}

	if (m_spInputComponent->GetButton(Input::Buttons::B) & m_spInputComponent->EXIT)
	{
		if (IsSnipe())
		{
			POINT center = { 1280 / 2,720 / 2 };

			Matrix mView = m_spCameraComponent->GetViewMatrix();
			Matrix mProj = m_spCameraComponent->GetProjMatrix();

			Vector3 NearPos = Scene::GetInstance().ConvertScreenToWorld(center.x, center.y, 0.0f, 1280, 720, mView, mProj);
			Vector3 FarPos = Scene::GetInstance().ConvertScreenToWorld(center.x, center.y, 1.0f, 1280, 720, mView, mProj);

			Vector3 Angle = FarPos - NearPos;

			Angle.Normalize();

			auto m_spArrow = std::make_shared<Arrow>();

			m_spArrow->m_pos = m_mWorld.GetTranslation() + ArmL->m_localTransform.GetTranslation();
			m_spArrow->m_rot = m_spCameraComponent->GetCameraMatrix().GetAngles();
			m_spArrow->Deserialize(ResFac.GetJSON("Data/JsonFile/Object/Arrow.Json"));

			m_spArrow->Position(Angle);

			m_spArrow->SetOwner(shared_from_this());
			Scene::GetInstance().AddObject(m_spArrow);

			//カメラを戻す

			m_CameraTrans = m_memory;

			m_spCameraComponent->OffsetMatrix().CreateTranslation(m_CameraTrans);
			m_spCameraComponent->OffsetMatrix().RotateX(25.0f * Radians);

			IsSnipe() = false;
		}
	}


	if (IsSnipe())
	{
		if (ArmL)
		{
			Vector3 vec;
			vec.x = ArmL->m_localTransform.GetTranslation().x;
			vec.y = ArmL->m_localTransform.GetTranslation().y;
			vec.z = ArmL->m_localTransform.GetTranslation().z;

			Matrix mat;

			mat.RotateY(m_rot.y);
			vec.x += m_pos.x;
			vec.y += m_pos.y;
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


			mat.RotateX(2);
			vec.x += Body->m_localTransform.GetTranslation().x + m_pos.x;
			vec.y += Body->m_localTransform.GetTranslation().y + m_pos.y;
			vec.z += Body->m_localTransform.GetTranslation().z + m_pos.z;

			mat.SetTranslation(vec);
			m_spCrossbow->SetMatrix(mat);
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
		if (obj->GetTag() & TAG_ActiveObject)
		{
			SphereResult sphereResult;

			m_gravityFlg = false;

			if (obj->HitCheckBySphereToMesh(sphereInfo, sphereResult))
			{
				push += sphereResult.m_push;
			}
		}
		else
		{
			m_gravityFlg = true;
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
		if (!(obj->GetTag() & TAG_Enemy)) { continue; }

		// 当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Explosion(Body->m_localTransform.GetTranslation() + m_pos);
			m_Hp--;
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


