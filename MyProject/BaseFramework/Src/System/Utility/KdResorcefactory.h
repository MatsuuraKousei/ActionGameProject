#pragma once

// =================================
// リソース管理クラス
// ・デザインpatternのFlyweightパターンを採用
// =================================
class KdResorceFactory
{
public:
	// モデルデータ取得
	std::shared_ptr<KdModel> GetModel(const std::string& filename);

	// テクスチャデータ所得
	std::shared_ptr<KdTexture> GetTexture(const std::string& filename);

	// JSON取得
	json11::Json GetJSON(const std::string& filename);

	// 管理を破棄する
	void Clear()
	{
		m_modelMap.clear();
		m_texMap.clear();
		m_jsonMap.clear();
	}

private:

	// JSON読み込み
	json11::Json LoadJSON(const std::string& filename);

	// モデルデータの管理マップ(連想配列？)
	std::unordered_map<std::string,		// first
		std::shared_ptr<KdModel>		// second
	> m_modelMap;

	// テクスチャ管理マップ
	std::unordered_map<std::string, std::shared_ptr<KdTexture>> m_texMap;

	// JSON管理マップ
	std::unordered_map< std::string, json11::Json > m_jsonMap;

	// =================================
	// シングルトン
	// =================================
private:
	KdResorceFactory(){}
public:
	static KdResorceFactory& GetInstance()
	{
		static KdResorceFactory instance;
		return instance;
	}
};

// これでKdResFacと書くだけで KdResorceFactory::GetInstance() と同じ内容を書いたことになる
#define KdResFac KdResorceFactory::GetInstance()