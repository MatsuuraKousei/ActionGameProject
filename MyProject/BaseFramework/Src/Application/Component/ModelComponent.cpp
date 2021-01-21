#include "ModelComponent.h"

// モデルセット
void ModelComponent::SetModel(const std::shared_ptr<Model>& rModel)
{
	// 使用しているモデルをセット
	m_spModel = rModel;

	// 念のため、コピー用配列のクリア
	m_coppiedNodes.clear();

	// ノードのコピー
	if (rModel)
	{
		m_coppiedNodes = rModel->GetOriginalNodes();
	}
}

void ModelComponent::Draw()
{
	// 有効じゃない時はスキップ
	if (m_enable = false) { return; }

	// モデルが無いときはスキップ
	if (m_spModel == nullptr) { return; }

	//// 試しにこれを書くと変化が試せてわかりやすい
	//m_dissolveThreshold += 0.001f;

	// 描画設定
	SHADER.m_modelShader.SetDissolveThreshold(m_dissolveThreshold);

	SHADER.m_modelShader.SetToDevice();

	// 全てのノードを一つ一つ描画
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];
		if (rNode.m_spMesh == nullptr) { continue; }
		// 行列セット
		SHADER.m_modelShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());

		Matrix mView;
		Matrix mProj;

		DirectX::BoundingOrientedBox obb;
		DirectX::BoundingOrientedBox::CreateFromBoundingBox(obb, rNode.m_spMesh->GetBoundingBox());

		DirectX::BoundingFrustum vf;
		DirectX::BoundingFrustum::CreateFromMatrix(vf, mProj);

		Matrix mCam = mView;
		mCam.Inverse();
		vf.Origin = mCam.GetTranslation();
		vf.Orientation = Quaternion().CreateFromMatrix(mCam);

		if (vf.Intersects(obb))
		{
			// 描画
		}
		SHADER.m_modelShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}

	//------------------------
	// 輪郭描画
	//------------------------
	// 表面をカリング(非表示)にするラスタライザステートをセット
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullFront);
	//輪郭シェーダーの情報を設定.
	SHADER.m_modelShader.SetToDevice_Outline();

	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];

		// メッシュがない場合はスキップ
		if (rNode.m_spMesh == nullptr) { continue; }

		// 行列セット
		SHADER.m_modelShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());

		// 描画
		SHADER.m_modelShader.DrawMesh_Outline(rNode.m_spMesh.get());
	}
	// 裏面をカリング(非表示)にするラスタライザステートに戻す
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);

}

void ModelComponent::DrawShadowMap()
{
	// 有効じゃないときはスキップ
	if (m_enable == false)return;
	// モデルがないときはスキップ
	if (m_spModel == nullptr)return;


	// 全ノード(メッシュ)を描画
	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];

		// メッシュがない場合はスキップ
		if (rNode.m_spMesh == nullptr) { continue; }

		// 行列セット
		SHADER.m_genShadowMapShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());

		// 描画
		SHADER.m_genShadowMapShader.DrawMeshDepth(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}

}
