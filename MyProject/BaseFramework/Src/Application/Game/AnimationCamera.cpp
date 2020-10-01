#include "AnimationCamera.h"
#include"./Scene.h"
#include"../Component/CameraComponent.h"

void AnimationCamera::Update()
{
	//補完する対象がどちらもいるか
	auto start = m_wpStart.lock();
	auto end = m_wpEnd.lock();
	if (!start || !end)
	{
		//補完する必要がなかった
		m_alive = false;//自信も破棄
		return;
	}

	//補完する行列の取得
	auto& mStart = start->GetCameraComponent()->GetCameraMatrix();
	auto& mEnd = end->GetCameraComponent()->GetCameraMatrix();

	//座標の保管
	const Vector3& vStart = mStart.GetTranslation();
	const Vector3& vEnd = mEnd.GetTranslation();

	Vector3 vTo = vEnd - vStart;		//ゴール地点へのベクトル
	Vector3 vNow = vStart + vTo * m_progress;	//進行具合を加味して座標を求める

	//開始地点と終了地点のクォーターニオンを生成（行列→回転）
	DirectX::XMVECTOR qSt = DirectX::XMQuaternionRotationMatrix(mStart);
	DirectX::XMVECTOR qEd = DirectX::XMQuaternionRotationMatrix(mEnd);

	//クォーターニオンを使って回転の補完
	DirectX::XMVECTOR qOTW = DirectX::XMQuaternionSlerp(qSt, qEd, m_progress);

	//クォーターニオンを回転行列に変換（回転行列）
	Matrix mRot = DirectX::XMMatrixRotationQuaternion(qOTW);

	//カメラに設定する行列
	Matrix mCam = mRot;
	mCam.SetTranslation(vNow);

	//カメラへセット
	m_spCameraComponent->SetCameraMatrix(mCam);

	//進行具合の更新
	m_progress += m_speed;
	if (m_progress > 1.0f)
	{
		m_progress = 1.0f;
		m_alive = false;	//自身を破棄

		//ゴール地点まで補完し終わったので、カメラターゲットをゴール地点のキャラクターにする
		Scene::GetInstance().SetTargetCamera(end->GetCameraComponent());
	}
}
