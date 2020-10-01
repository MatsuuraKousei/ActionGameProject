#include "System/System.h"

#include "Utility.h"

// viewから画像情報を取得する
void GetTextuxxreInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}

void MergePrefab(json11::Json& rSrcJson)
{
    // プレハブ指定ありの場合は、プレハブ側のものをベースにこのJSONをマージする
        // プレハブの中のものを読み込んだ後shootingGame.jsonで上書きしている?
    std::string prefabFilename = rSrcJson["Prefab"].string_value();

    // 文字列が入っていれば
    if (prefabFilename.size() > 0)
    {
        // JSON読み込み
        json11::Json prefJson = ResFac.GetJSON(prefabFilename);

        if (prefJson.is_null() == false)
        {
            // マージする
            for (auto&& n : rSrcJson.object_items())
            {
                prefJson[n.first] = n.second;
            }

            // マージしたものに差し替え
            rSrcJson = prefJson;
        }
    }
}
