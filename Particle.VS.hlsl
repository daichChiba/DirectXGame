#include "Particle.hlsli"

struct TransformationMatrix{
    Matrix4x4 WVP;
    Matrix4x4 World;
};

struct ParticleForGPU{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 color;
};
StructuredBuffer<ParticleForGPU> gParticle : register(t0);



struct VertexShaderInput{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId:SV_InstanceID){
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixs[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceId].color;
    //output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.World));

    return output;
}