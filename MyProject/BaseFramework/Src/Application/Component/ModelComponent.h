#pragma once

#include "../Game/GameObject.h"

// =====================================
// モデルコンポーネント
// =====================================
class ModelComponent
{
public:
	ModelComponent(GameObject& owner):m_owner(owner){}

	// 有効フラグ
	bool IsEnable() const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

	// アニメーションデータ取得
	const std::shared_ptr<AnimationData> GetAnimation(const std::string& animName) const
	{
		if (!m_spModel) { return nullptr; }
		return m_spModel->GetAnimation(animName);
	}

	// モデル取得
	// オリジナルを返すのではなくコピーしたものを返す
	const std::vector<Model::Node>& GetNodes() const { return m_coppiedNodes; }
	std::vector<Model::Node>& GetChangebleNodes() { return m_coppiedNodes; }

	// メッシュ取得
	// オリジナルを返すのではなくコピーしたものを返す
	inline const std::shared_ptr<Mesh> GetMesh(UINT index)const
	{
		if (index >= m_coppiedNodes.size()) { return nullptr; }	// 引数がサイズより大きかったら抜ける
		return m_coppiedNodes[index].m_spMesh;
	}

	// ノードを探して返す
	inline Model::Node* FileNode(const std::string& name)
	{
		// コピーしたノードの中から検索をかける
		for (auto&& node : m_coppiedNodes)
		{
			if (node.m_name == name)
			{
				return &node;
			}
		}
	}

	// モデルセット
	//inline void SetModel(const std::shared_ptr<KdModel>& model) { m_spModel = model; }
	void SetModel(const std::shared_ptr<Model>& rModel);

	// StandardShaderで描画
	void Draw();

	// シャドウマップ生成描画
	void DrawShadowMap();

	// ディゾルブの閾値設定
	void SetDissolveThreshold(float threshold)
	{
		m_dissolveThreshold = threshold;
	}

private:

	std::vector<Model::Node> m_coppiedNodes;	// 個別管理のため、オリジナルからコピーして保持する配列

	// 有効
	bool m_enable = true;

	// モデルデータの参照
	std::shared_ptr<Model> m_spModel;

	GameObject& m_owner;

	// 描画設定
	float			m_dissolveThreshold = 0.0f;
};