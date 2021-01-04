#include "Model.h"
#include "GLTFLoader.h"

// コンストラクタ
Model::Model() //:m_pMesh(nullptr)
{

}

// デストラクタ
Model::~Model()
{
	Release();
}


// ロード関数
bool Model::Load(const std::string& filename)
{
	// ファイルの完全パスを取得
	std::string fileDir = GetDirFromPath(filename);

	// GLTFの読み込み
	std::shared_ptr<GLTFModel> spGltfModel = LoadGLTFModel(filename);
	if (spGltfModel == nullptr) { return false; }

	// ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	// メッシュの受け取り
	for (UINT i = 0; i < spGltfModel->Nodes.size(); ++i)	// Nodes.sizeはパーツ　腕とか足とかがある分だけ回す
	{
		// 入力元ノード
		const GLTFNode& rSrcNode = spGltfModel->Nodes[i];

		// 出力先ノード
		Node& rDstNode = m_originalNodes[i];

		// ノード情報のセット
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;

		// ノードの内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			// リストにメッシュのインスタンス化
			rDstNode.m_spMesh = std::make_shared<Mesh>();

			if (rDstNode.m_spMesh)
			{
				rDstNode.m_spMesh->Create(
					//		頂点情報配列		面情報配列			サブセット情報配列
					rSrcNode.Mesh.Vertices, rSrcNode.Mesh.Faces, rSrcNode.Mesh.Subsets);
			}
		}
	}

	// マテリアル配列を受け取れるサイズのメモリを確保
// ↓最初から必要な分を確保する 使う数がわかっているのならこのほうがいい
	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		// src = sourceの略
		// dst = destinationの略　受け取り
		const GLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];		// 渡す側
		Material& rDstMaterial = m_materials[i];						// 受け取る側

		// 名前
		rDstMaterial.Name = rSrcMaterial.Name;

		// 基本色
		// 二つ用意することでプログラムの中で動的に変えることができる
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<Texture>();

		// テクスチャ読み込み
		rDstMaterial.BaseColorTex = ResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)	// nullが帰ってきたら
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();	// 真っ白のものを読み込む
		}

		// 金属性・粗さ
		rDstMaterial.Metallic = rSrcMaterial.Metallic;
		rDstMaterial.Roughness = rSrcMaterial.Roughness;
		rDstMaterial.MetallicRoughnessTex = std::make_shared<Texture>();
		if (rDstMaterial.MetallicRoughnessTex->Load(fileDir + rSrcMaterial.MetallicRoughnessTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			rDstMaterial.MetallicRoughnessTex = D3D.GetWhiteTex();
		}

		// 法線マップ
		rDstMaterial.NormalTex = std::make_shared<Texture>();
		if (rDstMaterial.NormalTex->Load(fileDir + rSrcMaterial.NormalTexture) == false)
		{
			// 読み込めなかった場合は、代わりにZ向き法線マップを使用
			rDstMaterial.NormalTex = D3D.GetNormalTex();
		}

	}

	//アニメーションデータ
	m_spAnimations.resize(spGltfModel->Animations.size());

	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		const GLTFAnimationData& rSrcAnimation = *spGltfModel->Animations[i];

		m_spAnimations[i] = std::make_shared<AnimationData>();
		AnimationData& rDstAnimation = *(m_spAnimations[i]);

		rDstAnimation.m_name = rSrcAnimation.m_name;

		rDstAnimation.m_maxLength = rDstAnimation.m_maxLength;

		rDstAnimation.m_nodes.resize(rDstAnimation.m_nodes.size());

		for (UINT i = 0; i < rDstAnimation.m_nodes.size(); ++i)
		{
			rDstAnimation.m_nodes[i].m_nodeOffset = rSrcAnimation.m_nodes[i]->m_nodeOffset;
			rDstAnimation.m_nodes[i].m_translations = rSrcAnimation.m_nodes[i]->m_translations;
			rDstAnimation.m_nodes[i].m_rotations = rSrcAnimation.m_nodes[i]->m_rotations;
		}
	}

	return true;
}

const std::shared_ptr<AnimationData> Model::GetAnimation(const std::string& animName) const
{
	for (auto&& anim : m_spAnimations)
	{
		if (anim->m_name == animName)
		{
			return anim;
		}
	}
}

// 解放
void Model::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}
