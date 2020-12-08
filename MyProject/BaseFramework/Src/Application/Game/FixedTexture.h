#pragma once

#include "GameObject.h"

class FixedTexture : public GameObject
{
public:
	FixedTexture();
	~FixedTexture();

	// angle : Z軸の回転角度
	void SetInfo(const std::shared_ptr<Texture>& rTex, float size, float angle);

	// アニメーションの更新
	virtual void Update() override;

	// 半透明物の描画
	virtual void DrawEffect() override;

private:
	// 四角形ポリゴン
	SquarePolygon		m_poly;

	int					m_time = 1;

	// Z軸の回転
	float				m_angleZ = 0;

	float               m_animSpeed = 0;
};