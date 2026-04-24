

struct VSInput
{
    float3 inPosition;

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
};

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(transformUBO.proj, mul(transformUBO.view, mul(transformUBO.model, float4(input.inPosition, 1.0))));
    return output;
}

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return float4(1, 1, 1, 1.0);
}