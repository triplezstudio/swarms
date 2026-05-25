import shader_interfaces;

struct VSInput
{
    float3 inPosition;
    float2 inTexcoord;

};

[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(perObjectUBO.model, float4(input.inPosition, 1.0))));
    output.fragTexcoord = input.inTexcoord;
    output.textureId = perObjectUBO.textureId;
    output.color = perObjectUBO.color;
    return output;
}
