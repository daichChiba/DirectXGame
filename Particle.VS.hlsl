struct TransformationMatrix{
    float32_t4x4 WVP;
    float32_t4x4 World;
};
StructuredBuffer<TransformationMatrix> gTransformationMatrixs : register(t0);



struct VertexShaderInput{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId:SV_InstanceID){
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixs[instanceId].WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.World));

    return output;
}