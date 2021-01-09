// カメラやライトのデータが使えるようになる
#include "../inc_Common.hlsli"
// 共通データ
#include "inc_ModelShader.hlsli"

//==================================
// ピクセルシェーダー　１つ１つのピクセルがやってくる
//==================================
float4 main(VSOutput In) : SV_Target0
{
    // 単色で返す
    return float4(0,0,0,1);
}
