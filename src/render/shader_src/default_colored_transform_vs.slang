import shader_interfaces;

struct VSInput
{
    float3 inPosition;

};


[shader("vertex")]
VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(perObjectUBO.model, float4(input.inPosition, 1.0))));
    output.color = perObjectUBO.color;
    return output;
}
