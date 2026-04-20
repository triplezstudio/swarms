

struct VSInput
{
    float3 inPosition;
    float3 inColor;
    float2 inTexcoord;

};

[[vk::binding(0,0)]]
struct TransformData
{
     float4x4 model;
     float4x4 view;
     float4x4 proj;

};
ConstantBuffer<TransformData> transformUBO;

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
    float2 fragTexcoord;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(transformUBO.proj, mul(transformUBO.view, mul(transformUBO.model, float4(input.inPosition, 1.0))));
    //output.pos =  mul(transformUBO.view,mul(transformUBO.model, float4(input.inPosition, 1.0)));
    output.color = input.inColor;
    output.fragTexcoord = input.inTexcoord;
    return output;
}

Sampler2D textureSampler;

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return textureSampler.Sample(vertIn.fragTexcoord);
}