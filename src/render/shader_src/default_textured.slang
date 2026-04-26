

struct VSInput
{
    float3 inPosition;
    float2 inTexcoord;

};

struct CameraData
{
    float4x4 view;
    float4x4 proj;

};
[[vk::binding(0,0)]]
ConstantBuffer<CameraData> cameraUBO;


struct PerObjectData
{
    float4x4 model;
    uint textureId;
    uint pad0;
    uint pad1;
    uint pad2;
};
[[vk::binding(0,1)]]
ConstantBuffer<PerObjectData> perObjectUBO;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexcoord;
    uint textureId;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(perObjectUBO.model, float4(input.inPosition, 1.0))));
    output.fragTexcoord = input.inTexcoord;
    output.textureId = perObjectUBO.textureId;
    return output;
}

[[vk::binding(0,2)]]
Sampler2D textureSampler[];

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return textureSampler[vertIn.textureId].Sample(vertIn.fragTexcoord);
}