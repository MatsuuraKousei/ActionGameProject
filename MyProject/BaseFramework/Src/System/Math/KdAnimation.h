#pragma once

//アニメーションキー（クォーターニオン
struct KdAnimKeyQuaternion
{
	float				m_time = 0;		//時間
	KdQuaternion		m_quat;			//クォーターニオンデータ
};

//アニメーションキー（ベクトル
struct KdAnimKeyVector3
{
	float				m_time = 0;		//時間
	KdVec3				m_vec;			//3Dベクトルデータ
};

//====================================
//アニメーションデータ
//====================================
struct KdAnimationData
{
	//アニメーション名
	std::string			m_name;
	//アニメの長さ
	float				m_maxLength = 0;
	//1ノードのアニメーションデータ
	struct Node
	{
		bool InterpolateTranslations(KdVec3& result, float time);
		bool InterpolateRotations(KdQuaternion& result, float time);

		int							m_nodeOffset = -1;	//対象モデルノードのOffset

		//各チャンネル
		std::vector<KdAnimKeyVector3>		m_translations;		//位置キーリスト
		std::vector<KdAnimKeyQuaternion>	m_rotations;		//回転キーリスト
	};

	//全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};