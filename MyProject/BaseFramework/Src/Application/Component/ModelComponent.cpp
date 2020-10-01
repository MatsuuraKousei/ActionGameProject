﻿#include "ModelComponent.h"

// モデルセット
void ModelComponent::SetModel(const std::shared_ptr<KdModel>& rModel)
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

	// 全てのノードを一つ一つ描画
	for (UINT i = 0; i < m_coppiedNodes.size();i++)
	{
		auto& rNode = m_coppiedNodes[i];
		if (rNode.m_spMesh == nullptr) { continue; }
		SHADER.m_standardShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());
		SHADER.m_standardShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}

}
