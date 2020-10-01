#include "EditorCamera.h"
#include "../main.h"

// コンストラクタ
EditorCamera::EditorCamera()
{
	m_mCam = Math::Matrix::CreateTranslation(0.0f, 0.0f, -5.0f);
	// X軸を軸にして20度下に傾ける？
	m_mCam *= Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(20));
	m_mCam *= Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(50));

	// 射影行列	(カメラをどういう風に写すか 射影行列の距離の中にあるオブジェクトだけ写される)
	m_mProj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60),		// 視野角(値が小さくなると見える範囲が狭くなる、大きくなると広くなる)
		D3D.GetBackBuffer()->GetAspectRatio(),	// アスペクト比(高さを1とした横比率)
		0.01f,									// 最近接距離(一番近く写せる距離)
		5000.0f);								// 最遠方距離(一番遠く写せる距離)

}

// デストラクタ
EditorCamera::~EditorCamera()
{

}

// 更新
void EditorCamera::Update()
{
	POINT nowPos;
	GetCursorPos(&nowPos);

	float ration = 1.0f;
	// Shiftで大きく
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { ration = 5.f; }
	// Controlで小さく
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) { ration = 0.2f; }


	// マウスの右ボタン
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// マウスの前回からの移動量
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.5f * ration;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.5f * ration;

		// カメラY軸回転
		Math::Matrix mRotateY;
		mRotateY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(deltaX));
		m_mCam *= mRotateY;

		// カメラX軸回転
		Math::Matrix mRotateX;
		mRotateX = DirectX::XMMatrixRotationAxis(m_mCam.Right(),DirectX::XMConvertToRadians(deltaY));
		m_mCam *= mRotateX;
	}

	// ホイールドラッグ(ホイールを押し込んだ状態で動かす)
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
	{
		// マウスの前回からの移動量
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.05f * ration;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.05f * ration;

		m_viewPos -= m_mCam.Right() * deltaX;	// m_mCam.Right()はカメラのX
		m_viewPos += m_mCam.Up() * deltaY;		// m_mCam.UP()はカメラのY
	}

	// ホイール回転(注視点の移動)
	// ホイールの回転をとる場合↓を使うので main.h をインクルードする
	if (APP.m_window.GetMouseWheelVal())
	{
		Math::Matrix mDistance;
		// doubleからfloatへの変換です。というエラーが出ている
		mDistance = DirectX::XMMatrixTranslation(0.0f, 0.0f, (float)APP.m_window.GetMouseWheelVal() * 0.01 * ration);

		//m_mCam *= mDistance;
		m_mCam = mDistance * m_mCam;
	}


	// 前フレームの座標の中に今の座標を入れる
	m_prevMousePos = nowPos;
}

// ビュー行列・射影行列をシェーダーへセット
void EditorCamera::SetToShader()
{
	// カメラ行列の作成　　↓カメラの回転が入ってる
	Math::Matrix mCam = m_mCam;

	// カメラ行列に注視点の座標を合成する　カメラの場所
	mCam *= DirectX::XMMatrixTranslation(m_viewPos.x, m_viewPos.y, m_viewPos.z);

	// カメラの座標をシェーダーにセット
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();

	// カメラのビュー行列をシェーダーにセット 
	SHADER.m_cb7_Camera.Work().mV = mCam.Invert();

	// カメラの射影行列
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	// カメラ情報(ビュー行列、射影行列)を、各シェーダーの定数バッファへセット
	SHADER.m_cb7_Camera.Write();

}
