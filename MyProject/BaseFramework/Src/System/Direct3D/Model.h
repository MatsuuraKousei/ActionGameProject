#pragma once		// 多重インクルード防止

struct AnimationData;

class Model
{
public:

	Model();
	~Model();

	bool Load(const std::string& filename);

	// アクセサ
	// 他の場所でも見れるようにする
	// {}の前にconstを書くことで中身の値が変更されませんよという事を明示的に書くことができる
	const std::shared_ptr<Mesh>  GetMesh(UINT index) const
	{
		// 渡された引数がメッシュのサイズを超えていたらnullptr　超えていないならメッシュ情報を渡す
		//					前提の条件が	　? 正しかったら				　　: 正しくなかったら
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr;
	}
	const std::vector<Material>& GetMaterials() const { return m_materials; }

	// ノード：モデルを形成するメッシュを扱うための最小単位
	// 構造体の中に入れておくことで名前かぶりなどを防ぐ
	struct Node
	{
		std::string					m_name;				// ノード名　
		Matrix						m_localTransform;	// 変更行列(原点からどれだけ離れているか)
		std::shared_ptr<Mesh>		m_spMesh;			// メッシュ情報
	};

	// 文字列を元にノードの検索
	inline Node* FileNode(const std::string& name)
	{
		for (auto&& node : m_originalNodes)
		{
			if (node.m_name == name) { return &node; }
		}
		return nullptr;
	}

	// ノード配列取得
	// リストの参照を返している
	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }

	//アニメーションデータの取得
	const std::shared_ptr<AnimationData> GetAnimation(const std::string& animName) const;

private:
	void Release();								// 解放
	std::vector<Node>		m_originalNodes;	// データのノード配列

	// マテリアル配列
	std::vector<Material> m_materials;		// マテリアルを複数持つので複数形になっている vectorを使うとメモリの許す範囲で配列を増やしたり減らしたりできる

	//アニメーションデータリスト
	std::vector<std::shared_ptr<AnimationData>>	m_spAnimations;

};