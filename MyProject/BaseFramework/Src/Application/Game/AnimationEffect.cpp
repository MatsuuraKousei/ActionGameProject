#include "AnimationEffect.h"

// コンストラクタ
AnimationEffect::AnimationEffect() {}
// デストラクタ
AnimationEffect::~AnimationEffect() {}

// アニメーション情報の設定(初期化)
void AnimationEffect::SetAnimationInfo(const std::shared_ptr<Texture>& rTex, float size, int splitX, int splitY, float angle,float speed)
{
	// ポリゴンの大きさと色を設定
	m_poly.Init(size, size, { 1,1,1,1 });

	// アニメーションの分割数を設定
	m_poly.SetAnimationInfo(splitX, splitY);

	// アニメーションの位置を0コマ目に移動しておく
	m_poly.SetAnimationPos(0);

	// 渡されたテクスチャを設定する
	m_poly.SetTexture(rTex);

	// Z軸回転角度を覚える
	m_angleZ = angle;

	// アニメの速度
	m_animSpeed = speed;
}

// アニメーションの更新
void AnimationEffect::Update()
{
	// アニメーション進行
	// 引数で渡している速度はInfoで値を設定したほうがいいかも
	m_poly.Animation(m_animSpeed, false);

	// アニメ終了
	if (m_poly.IsAnimationEnd())
	{
		Destroy();
	}
}

// 半透明物の描画
void AnimationEffect::DrawEffect()
{
	// 各軸方向の拡大率を取得
	float scaleX = m_mWorld.GetAxisX().Length();
	float scaleY = m_mWorld.GetAxisY().Length();
	float scaleZ = m_mWorld.GetAxisZ().Length();

	// ビルボード処理
	Matrix drawMat;
	drawMat.CreateScalling(scaleX, scaleY, scaleZ);	// 拡大率を設定
	drawMat.RotateZ(m_angleZ * Radians);		// Z軸の回転角度を加える

	// カメラの行列の取得
	Matrix camMat = SHADER.m_cb7_Camera.GetWork().mV;

	// 座標は自分のものを使う
	drawMat.SetTranslation(m_mWorld.GetTranslation());

	// 描画行列にビルボード回転(ここが置き換わります)
	drawMat.SetBillboard(SHADER.m_cb7_Camera.GetWork().mV);

	// 描画
	SHADER.m_effectShader.SetWorldMatrix(drawMat);
	SHADER.m_effectShader.WriteToCB();
	m_poly.Draw(0);

}
