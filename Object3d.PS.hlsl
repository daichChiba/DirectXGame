#include "object3d.hlsli"

struct Material{
    float32_t4 color;
    int32_t enableLighting;
};

//struct DirectionalLight{
//    float32_t4 color;//!<ライトの色
//    float32_t3 direction;//!<ライトの向き
//    float intensity;//!<輝度
//};

ConstantBuffer<Material> gMaterial : register(b0);
//ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = gMaterial.color*textureColor;
    //if (gMaterial.enableLighting != 0){
    //    float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    //    output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    //}
    //else{//Lightingしない場合。前回までと同じ演算
    //    output.color = gMaterial.color * textureColor;
    //}
    
    //textureのα値が0.5以下の時にPixelを棄却
    if (textureColor.a <= 0.5){
        discard;
    }
    //textureのα値が0の時にPixelを棄却
    if (textureColor.a == 0.0){
        discard;
    }
    //output.colorのα値が0の時にPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}