#pragma once 

class TrailPolygon
{
public:
	// 先頭のポイントを取得
	inline Math::Matrix* GetTopPoint()
	{
		if (m_pointList.size() == 0) { return nullptr; }
		return &m_pointList.front();
	}

	// ポイントを追加
	inline void AddPoint(const Math::Matrix& mat)
	{
		// push_frontで配列の先頭に追加している
		m_pointList.push_front(mat);
	}

	// 最後尾のポイントを削除
	inline void DelPoint_Back()
	{
		// 配列の一番後ろを削除
		m_pointList.pop_back();
	}

	// リストの数を取得
	inline int GetNumPoints() const
	{
		return (int)m_pointList.size();
	}

	// 画像をセット
	inline void SetTexture(const std::shared_ptr<Texture> & tex)
	{
		m_texture = tex;
	}

	// 通常描画　行列のX軸方向に頂点が作られ描画される
	// ・width 　・・・ポリゴンの幅
	void Draw(float width);

	// ビルボード描画
	void DrawBillboard(float width);

	// 頂点情報をそのまま繋げてポリゴンを描画
	void DrawStrip();
private:

	// 煙　軌跡
	float m_trailRotate = 0.0f;

	// 1頂点の形式
	struct Vertex
	{
		Math::Vector3 Pos;	// 座標
		Math::Vector2 UV;	// 画像の場所
		Math::Vector4 Color = { 1,1,1,1 };	// 板ポリの色
	};

	// 軌跡の位置などを記憶する行列配列
	// deque(デキュー)の特徴は先頭に追加して最後尾を削除する
	std::deque<Math::Matrix>	m_pointList;

	// テクスチャ
	std::shared_ptr<Texture>  m_texture;
};