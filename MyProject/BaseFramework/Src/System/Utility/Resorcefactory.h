#pragma once

// =================================
// リソース管理クラス
// ・デザインpatternのFlyweightパターンを採用
// =================================
class ResorceFactory
{
public:
	// モデルデータ取得
	std::shared_ptr<Model> GetModel(const std::string& filename);

	// テクスチャデータ所得
	std::shared_ptr<Texture> GetTexture(const std::string& filename);

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
		std::shared_ptr<Model>		// second
	> m_modelMap;

	// テクスチャ管理マップ
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_texMap;

	// JSON管理マップ
	std::unordered_map< std::string, json11::Json > m_jsonMap;

	// =================================
	// シングルトン
	// =================================
private:
	ResorceFactory(){}
public:
	static ResorceFactory& GetInstance()
	{
		static ResorceFactory instance;
		return instance;
	}
};

// これでKdResFacと書くだけで KdResorceFactory::GetInstance() と同じ内容を書いたことになる
#define ResFac ResorceFactory::GetInstance()