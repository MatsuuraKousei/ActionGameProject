#include "EffectObject.h"

// 更新
void EffectObject::Update()
{
	// 表示フラグがたっていなかったら更新しない
	if (m_alive == false) { return; }

	// 寿命の更新
	if (--m_lifeSpan <= 0)
	{
		Destroy();	// 消す
		return;
	}
	// 徐々に大きくなっていく更新
	{
		m_scale += m_lifeSpan * 0.001f;

		// 色を爆発っぽい色(黄色、オレンジ)
		m_color.R(m_lifeSpan / 30.0f);
		m_color.G(m_lifeSpan / 60.0f);
	}
}

#include "Scene.h"
// 描画
void EffectObject::Draw()
{
	// 表示フラグがたっていなかったら更新しない
	if (m_alive == false) { return; }

	// デバッグ表示で爆発の球を描く
	Scene::GetInstance().AddDebugSphereLine(m_mWorld.GetTranslation(), m_scale, m_color);
}
