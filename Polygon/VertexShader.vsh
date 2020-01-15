///////////////////////////////////////////////////////
// グローバル
///////////////////////////////////////////////////////
cbuffer ConstBuffer : register(b0)
{
    int2 gCenterWindow;
}

///////////////////////////////////////////////////////
// 構造体宣言ブロック
///////////////////////////////////////////////////////
struct vertexInput
{
    int2 pos : POSITION0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
};

struct vertexOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
};

///////////////////////////////////////////////////////
// 頂点シェーダプログラムブロック
///////////////////////////////////////////////////////
vertexOutput vs_main(vertexInput inData)
{
    vertexOutput outData;
    
    //クライアント領域中心～描画したい座標までの距離
//    outData.pos[0] = (asfloat(inData.pos[0]) - asfloat(gCenterWindow[0])) / asfloat(gCenterWindow[0]);
//    outData.pos[1] = (asfloat(gCenterWindow[1]) - asfloat(inData.pos[1])) / asfloat(gCenterWindow[1]);
    
    float2 center = float2(gCenterWindow[0], gCenterWindow[1]);
    float2 vertex = float2(inData.pos[0], inData.pos[1]);
    
    outData.pos[0] = (vertex[0] - center[0]) / center[0];
    outData.pos[1] = (center[1] - vertex[1]) / center[1];
    outData.pos[2] = 1.0f;
    outData.pos[3] = 1.0f;
    
    outData.col = inData.col;
    outData.tex = inData.tex;
    
    return outData;
}
