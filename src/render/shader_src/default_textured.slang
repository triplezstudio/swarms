

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


struct TransformData
{
     float4x4 model;
};
[[vk::binding(0,1)]]
ConstantBuffer<TransformData> transformUBO;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexcoord;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(transformUBO.model, float4(input.inPosition, 1.0))));
    //output.pos =  mul(transformUBO.view,mul(transformUBO.model, float4(input.inPosition, 1.0)));
  
    output.fragTexcoord = input.inTexcoord;
    return output;
}

[[vk::binding(0,2)]]
Sampler2D textureSampler;

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return textureSampler.Sample(vertIn.fragTexcoord);
}