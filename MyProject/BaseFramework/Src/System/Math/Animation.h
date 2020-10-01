#pragma once

//アニメーションキー（クォーターニオン
struct AnimKeyQuaternion
{
	float				m_time = 0;		//時間
	Quaternion		m_quat;			//クォーターニオンデータ
};

//アニメーションキー（ベクトル
struct AnimKeyVector3
{
	float				m_time = 0;		//時間
	Vector3				m_vec;			//3Dベクトルデータ
};

//====================================
//アニメーションデータ
//====================================
struct AnimationData
{
	//アニメーション名
	std::string			m_name;
	//アニメの長さ
	float				m_maxLength = 0;
	//1ノードのアニメーションデータ
	struct Node
	{
		bool InterpolateTranslations(Vector3& result, float time);
		bool InterpolateRotations(Quaternion& result, float time);

		int							m_nodeOffset = -1;	//対象モデルノードのOffset

		//各チャンネル
		std::vector<AnimKeyVector3>		m_translations;		//位置キーリスト
		std::vector<AnimKeyQuaternion>	m_rotations;		//回転キーリスト
	};

	//全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};