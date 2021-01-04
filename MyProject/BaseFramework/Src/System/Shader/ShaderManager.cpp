#include "System/System.h"
#include "ShaderManager.h"

void ShaderManager::Init()
{

	//============================================
	// シェーダ
	//============================================

	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();
	// 3Dモデル描画シェーダー
	m_modelShader.Init();

	//============================================
	// 定数バッファ
	//============================================

	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV = Math::Matrix::CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().CamPos = m_cb7_Camera.Work().mV.Translation();
	m_cb7_Camera.Work().mV = m_cb7_Camera.Work().mV.Invert();

	// 初期射影行列
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 1000);
	m_cb7_Camera.Write();

	// ライト
	m_cb8_Light.Create();
	m_cb8_Light.Write();
	D3D.GetDevContext()->VSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());


	//============================================
	// パイプラインステート関係
	//============================================

	// 深度ステンシルステート作成
	m_ds_ZEnable_ZWriteEnable = D3D.CreateDepthStencilState(true, true);		// 重なりをチェックする　描画もする
	m_ds_ZDisable_ZWriteDisable = D3D.CreateDepthStencilState(false, false);	// 重なりのチェックもしないし描画もしない

	m_ds_ZEnable_ZWriteDisable = D3D.CreateDepthStencilState(true, false);		// 重なりのチェックをして描画はしない？

	// ラスタライザステート作成
	m_rs_CullBack = D3D.CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);
	m_rs_CullNone = D3D.CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, false);

	// ブレンドステート
	m_bs_Alpha = D3D.CreateBlendState(BlendMode::Alpha);	// 半透明ブレンド
	m_bs_Add = D3D.CreateBlendState(BlendMode::Add);		// 加算ブレンド
}

// 解放
void ShaderManager::Release()
{
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();
	m_modelShader.Release();

	m_cb7_Camera.Release();
	m_cb8_Light.Release();

	// 深度ステンシルステート解放
	// これを使ったら安全にReleaseできる
	SafeRelease(m_ds_ZEnable_ZWriteEnable);
	SafeRelease(m_ds_ZDisable_ZWriteDisable);
	SafeRelease(m_ds_ZEnable_ZWriteDisable);

	// ラスタライザステート解放
	SafeRelease(m_rs_CullBack);
	SafeRelease(m_rs_CullNone);

	// ブレンドステート解散
	SafeRelease(m_bs_Alpha);
	SafeRelease(m_bs_Add);
}
