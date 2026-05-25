import shader_interfaces;

struct VSInput
{
    float3 inPosition;
    float2 inTexcoord;

};

[shader("vertex")]
VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{

    VSOutput output;
    PerInstanceData perInstanceData = instanceBuffer[instanceID];
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(perInstanceData.model, float4(input.inPosition, 1.0))));
    output.fragTexcoord = input.inTexcoord;
    output.textureId = perInstanceData.textureId;
    output.color = perInstanceData.color;
    return output;
}
