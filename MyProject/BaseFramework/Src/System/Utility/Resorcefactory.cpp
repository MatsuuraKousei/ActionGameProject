﻿#include "Resorcefactory.h"

// １回読み込んでいたら２回目以降は使いまわすようにする
std::shared_ptr<Model> ResorceFactory::GetModel(const std::string& filename)
{
	// filenameのモデルがあるか？
	auto itFound = m_modelMap.find(filename);
	// ない場合
	if (itFound == m_modelMap.end())
	{
		// 生成読み込み
		auto newModel = std::make_shared<Model>();
		if (newModel->Load(filename) == false)
		{
			// 読み込み失敗時は、nullを返す
			return nullptr;
		}
		// リスト(map)に登録
		m_modelMap[filename] = newModel;
		// リソースを返す
		return newModel;
	}
	// ある場合
	else
	{
		// sharedで返す
		return (*itFound).second;
		// firstがfilename secondがnewModel  らしい
	}
}

// テクスチャデータ所得
std::shared_ptr<Texture> ResorceFactory::GetTexture(const std::string& filename)
{
	// filenameのテクスチャがあるか？
	auto itFound = m_texMap.find(filename);
	// 無い場合
	if (itFound == m_texMap.end())
	{
		// 生成 & 読み込み
		auto newTexture = std::make_shared<Texture>();
		if (newTexture->Load(filename) == false)
		{
			// 読み込み失敗時はnullを返す
			return nullptr;
		}
		// リスト(map)に登録
		m_texMap[filename] = newTexture;
		// リソースを返す
		return newTexture;
	}
	else
	{
		return (*itFound).second;
		// secondの中にはテクスチャ情報が入っている
	}
}

json11::Json ResorceFactory::GetJSON(const std::string& filename)
{
	// filenameの物があるか？(検索)
	auto itFound = m_jsonMap.find(filename);
	// 無い場合
	if (itFound == m_jsonMap.end())
	{
		// JSONファイルを文字列として読み込む
		json11::Json json = LoadJSON(filename);
		if (json.is_null())
		{
			assert(0 && "[GetJSON]jsonファイルが見つからない");

			return nullptr;
		}

		// 登録
		m_jsonMap[filename] = json;

		// 返す
		return json;
	}
	// ある場合
	else
	{
		return (*itFound).second;
	}
}

// JSON読み込み
json11::Json ResorceFactory::LoadJSON(const std::string& filename)
{
	// Jsonファイルを開く
	std::ifstream ifs(filename);
	if (ifs.fail())
	{
		assert(0 && "Jsonファイルのパスが間違っています");
		return nullptr;
	}

	// 文字列として全読み込み
	// ファイルの最初から最後まで文字コードとして読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	// 文字列のJSONを解析(パース)する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0)
	{
		assert(0 && "読み込んだファイルのJson変換に失敗");
		return nullptr;
	}

	return jsonObj;
}