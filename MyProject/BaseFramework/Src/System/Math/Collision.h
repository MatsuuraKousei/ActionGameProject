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

//球体メッシュの当たり判定
bool SphereToMesh(
	const Math::Vector3& rSpherePos,		// 球の中心の場所
	float radius,							// 球の半径
	const Mesh& mesh,						// 判定するメッシュ情報
	const DirectX::XMMATRIX& matrix,		// 判定する相手の行列
	Math::Vector3& rPushedPos				// 当たっていた場合、押し返された球の中心点
);

void PointToBox(const Vector3& point, const DirectX::BoundingOrientedBox& obb, Vector3& outPos);


// 点 vs 三角形との最近接点を求める
// * p			… 点の座標
// * a			… 三角形の点１
// * b			… 三角形の点２
// * c			… 三角形の点３
// * outPut		… 最近接点の座標が入る
void PointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a,
	const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPut);