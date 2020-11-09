#pragma once

#include "GameObject.h"

class AnimationEffect : public GameObject
{
public:
	AnimationEffect();
	~AnimationEffect();

	// アニメーション情報の設定(初期化)
	// angle : Z軸の回転角度
	void SetAnimationInfo(const std::shared_ptr<Texture>& rTex, float size, int splitX, int splitY, float angle,float speed);

	// アニメーションの更新
	virtual void Update() override;

	// 半透明物の描画
	virtual void DrawEffect() override;

private:
	// 四角形ポリゴン
	SquarePolygon		m_poly;

	// Z軸の回転
	float				m_angleZ = 0;

	float               m_animSpeed = 0;
};