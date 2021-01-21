#include "Collision.h"

using namespace DirectX;	// これをヘッダーに書くとみんなが使えるようになっておかしくなるので注意

bool RayToMesh(const XMVECTOR& rRayPos, const XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const Matrix& rMatrix, RayResult& rResult)
{	// モデルの逆行列でレイを変換
	XMMATRIX invMat = DirectX::XMMatrixInverse(0, rMatrix);

	// レイの判定開始位置を逆変換
	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos, invMat);	

	// 発射方向は正規化されていないと正しく判定できないので正規化
	XMVECTOR rayDir = XMVector3TransformNormal(rRayDir, invMat);

	// 逆行列に拡縮が入っていると
	// レイが当たった距離にも拡縮が反映されてしまうので
	// 判定用の最大距離にも拡縮を反映させておく
	//float rayCheckRange = maxDistance * XMVector3Length(rayDir).m128_f32[0];
	float dirLength = XMVector3Length(rayDir).m128_f32[0];	// 拡縮を一度変数に格納して覚えておく(Normalizeするとわからなくなるため)
	float rayCheckRange = maxDistance * dirLength;

	rayDir = XMVector3Normalize(rayDir);

	//-------------------------------------------------------
	// ブロードフェイズ
	//   AABB = =Axis Aligned Bounding Box
	// 　比較的軽量なAABB  vs  レイな判定で、
	// 　あきらかにヒットしない場合は、これ以降の判定を中止
	//-------------------------------------------------------
	{
		// AABB vs レイ
		float AABBdist = FLT_MAX;
		if (rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		// 最大距離以降なら範囲外なので中止
		if (AABBdist > maxDistance) { return false; }
	}


	//-------------------------------------------------------
	// ナローフェイズ
	// 　レイ　vs　全ての面
	//-------------------------------------------------------
	bool ret = false;					// 当たったかどうか
	float closestDist = FLT_MAX;		// 当たった面の距離

	// 面情報の取得
	const MeshFace* pFaces = &rMesh.GetFaces()[0];					// 面情報の先頭を取得
	UINT faceNum = rMesh.GetFaces().size();								// 面の総数を取得

	// 全ての面(三角形)と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構築する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// レイと三角形の当たり判定
		float triDist = FLT_MAX;

		bool bHit = DirectX::TriangleTests::Intersects(
			rayPos,	// 発射場所
			rayDir,			// 発射方向(正規化済み)

			// 判定する三角形の頂点情報
			rMesh.GetVertexPositions()[idx[0]],
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],

			triDist			// 当たった場合の距離
		);

		// ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		// 最大距離内か
		if (triDist <= rayCheckRange) 
		{
			//return true; 
			ret = true;		// 当たったとする

			// 当たり判定でとれる距離は拡縮に影響ないので、実際の長さを計算する
			triDist /= dirLength;

			// 近いほうを優先して残す
			if (triDist < closestDist)
			{
				closestDist = triDist;		// 距離を更新
			}
		}
	}
	//return false;

	Vector3 hitMat = rRayPos;
	Vector3 hitDir = rRayDir;
	hitDir.Normalize();
	hitDir *= closestDist;
	hitMat.x += hitDir.x;
	hitMat.y += hitDir.y;
	hitMat.z += hitDir.z;
	rResult.m_distance = closestDist;
	rResult.m_hit = ret;
	rResult.rHitPos = hitMat;

	return ret;
}

bool SphereToMesh(const Math::Vector3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, Math::Vector3& rPushdPos)
{
	{
		BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		BoundingSphere bs(rSpherePos, radius);
		if (aabb.Intersects(bs) == false)return false;

	}

	bool bHit = false;

	const auto* pFaces = &mesh.GetFaces()[0];
	UINT faceNum = mesh.GetFaces().size();
	const Math::Vector3* vertics = &mesh.GetVertexPositions()[0];

	XMMATRIX invM = XMMatrixInverse(0, matrix);
	XMVECTOR QPos = rSpherePos;
	QPos = XMVector3TransformCoord(QPos, invM);

	float radiusSq = radius * radius;

	XMVECTOR scale;
	scale.m128_f32[0] = XMVector3Length(matrix.r[0]).m128_f32[0];
	scale.m128_f32[1] = XMVector3Length(matrix.r[1]).m128_f32[0];
	scale.m128_f32[2] = XMVector3Length(matrix.r[2]).m128_f32[0];
	scale.m128_f32[3] = 0;

	XMVECTOR nearPt;
	XMVECTOR vToCentor;

	for (UINT fi = 0; fi < faceNum; fi++)
	{
		PointToTriangle(QPos, vertics[pFaces[fi].Idx[0]], vertics[pFaces[fi].Idx[1]], vertics[pFaces[fi].Idx[2]], nearPt);

		vToCentor = QPos - nearPt;

		vToCentor *= scale;

		if (XMVector3LengthSq(vToCentor).m128_f32[0] <= radiusSq)
		{
			nearPt = XMVector3TransformCoord(nearPt, matrix);

			XMVECTOR v = XMVector3Normalize(vToCentor);
			v *= radius - XMVector3Length(vToCentor).m128_f32[0];

			v /= scale;

			QPos += v;

			bHit = true;
		}
	}
	if (bHit)
	{
		rPushdPos = XMVector3TransformCoord(QPos, matrix);
	}
	return bHit;
}

void PointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPut)
{
	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;

	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ab.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		outPut = a;		// 重心座標(1,0,0)
		return;
	}

	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ab.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		outPut = b;		// 重心座標(0,1,0)
		return;
	}

	// pがabの辺領域の中にあるかどうかチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		outPut = a + ab * v; // 重心座標(1-v,v,0)
		return;
	}

	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];;//ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6) {
		outPut = c; // 重心座標(0,0,1)
		return;
	}


	// pがacの辺領域の中にあるかどうかチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		outPut = a + ac * w; // 重心座標(1-w,0,w)
		return;
	}

	// pがbcの辺領域の中にあるかどうかチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		outPut = b + (c - b) * w; // 重心座標(0,1-w,w)
		return;
	}

	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPut = a + ab * v + ac * w; // = u*a + v*b + w*c, u = va*demon = 1.0f - v - w
}

void PointToBox(const Vector3& point, const DirectX::BoundingOrientedBox& obb, Vector3& outPos)
{
	using namespace DirectX;

	// OBBの回転(クォーターニオン)
	XMVECTOR obbQuat = XMLoadFloat4(&obb.Orientation);

	// 点をOBBのローカル座標系へ変換(これでOBBからAABBの判定にできる)
	Vector3 pointCenter = XMVector3InverseRotate(point - Vector3(obb.Center), obbQuat);

	// Boxの最近接点を求める
	outPos = { 0,0,0 };
	for (int i = 0; i < 3; i++)
	{
		float dist = (&pointCenter.x)[i];
		if ((&pointCenter.x)[i] > (&obb.Extents.x)[i])
		{
			dist = (&obb.Extents.x)[i];
		}
		else if (dist < -(&obb.Extents.x)[i])
		{
			dist = -(&obb.Extents.x)[i];
		}

		(&outPos.x)[i] += dist;
	}

	// OBBのローカル座標系からワールドへ戻す
	outPos = XMVector3Rotate(outPos, obbQuat);
	outPos.x += obb.Center.x;
	outPos.y += obb.Center.y;
	outPos.z += obb.Center.z;
}