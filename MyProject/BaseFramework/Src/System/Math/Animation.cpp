#include "Animation.h"

//二分探索で、指定時間から次の配列要素のKeyIndexを求める関数
//list		…キー配列
//time		…時間
//戻り値	…次の配列要素のIndex
template<class T>
int BinarySearchNextAnimKey(const std::vector<T>& list, float time)
{
	int low = 0;
	int high = (int)list.size();
	while (low < high)
	{
		int mid = (low * high) / 2;
		float midTime = list[mid].m_time;

		if (midTime <= time)low = mid * 1;
		else high = mid;
	}
	return low;
}

bool AnimationData::Node::InterpolateTranslations(Vector3& result, float time)
{
	if (m_translations.size() == 0)return false;

	//キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_translations, time);

	//先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0)
	{
		result = m_translations.front().m_vec;
		return true;
	}
	//配列外キーなら、最後のデータを返す
	else if (keyIdx >= m_translations.size())
	{
		result = m_translations.back().m_vec;
		return true;
	}
	//それ以外（中間の時間）なら、その時間の値を補間計算で求める
	else
	{
		auto& prev = m_translations[keyIdx - 1];	//前のキー
		auto& next = m_translations[keyIdx];		//次のキー
		//前のキーと次のキーの時間から、０～１間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		//補間
		result = DirectX::XMVectorLerp(prev.m_vec, next.m_vec, f);
	}

	return true;
}

bool AnimationData::Node::InterpolateRotations(Quaternion& result, float time)
{
	if (m_rotations.size() == 0)return false;

	//キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_rotations, time);
	//先頭のキーなら、最後のデータを返す
	if (keyIdx == 0)
	{
		result = m_rotations.front().m_quat;
		return true;
	}
	//配列外キーなら、最後のデータを返す
	else if (keyIdx >= m_translations.size())
	{
		result = m_rotations.back().m_quat;
		return true;
	}
	//それ以外（中間の時間）なら、その時間の値を補間計算で求める
	else
	{
		auto& prev = m_rotations[keyIdx - 1];	//前のキー
		auto& next = m_rotations[keyIdx];		//次のキー
		//前のキーと次のキーの時間から、０～１間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		//補間
		result = DirectX::XMQuaternionSlerp(prev.m_quat, next.m_quat,f);
	}

	return true;
}