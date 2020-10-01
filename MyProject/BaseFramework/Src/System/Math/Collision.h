#pragma once

// レイ判定をしたときの結果情報
struct RayResult
{
	float	m_distance = FLT_MAX;	// 当たったところまでの距離
	bool	m_hit = false;		// 当たったかどうか
	Vector3  rHitPos = {};
};

// レイによる当たり判定
bool RayToMesh(
	const DirectX::XMVECTOR& rRayPos,
	const DirectX::XMVECTOR& rRayDir,
	float maxDistance,
	const Mesh& rMesh,const Matrix& rMatrix,
	RayResult& rResult
);