#pragma once

//====================================================
//
// システム系のヘッダーファイル
//
// 一回ここで読み込むとコンパイルが成功したことになるので２回目以降の読み込みが速くなる
//====================================================

// 便利機能
#include "Utility/Utility.h"

// 算術
#include "Math/Math.h"

// バッファ
#include "Direct3D/Buffer.h"
// Direct3D
#include "Direct3D/Direct3D.h"


// テクスチャ
#include "Direct3D/Texture.h"
// メッシュ
#include "Direct3D/Mesh.h"
// モデル
#include "Direct3D/Model.h"
// アニメーション
#include "Math/Animation.h"

// 板ポリゴン
#include "Direct3D/SquarePolygon.h"
// 軌跡ポリゴン
#include "Direct3D/TrailPolygon.h"

// 判定系
#include "Math/Collision.h"

// シェーダ
#include "Shader/ShaderManager.h"

// ウィンドウ
#include "Window/Window.h"

// リソース管理
#include "Utility/Resorcefactory.h"
