import shader_interfaces;



[shader("vertex")]
VSOutputLit main(VSInputPosNormal input, uint instanceID : SV_InstanceID)
{
    VSOutputLit output;
    PerInstanceData perInstanceData = instanceBuffer[instanceID];
    output.pos = mul(cameraUBO.proj, mul(cameraUBO.view, mul(perInstanceData.model, float4(input.inPosition, 1.0))));
    output.color = perInstanceData.color;
    output.normal = input.inNormal;
    return output;
}
