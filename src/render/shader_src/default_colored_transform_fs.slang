import shader_interfaces;


[shader("fragment")]
float4 main(VSOutputLit vertIn) : SV_TARGET
{
    float3 lightDir = float3(0.2, 1, 0.5);
    float diff = max(dot(normalize(vertIn.normal), lightDir), 0.0);
    return vertIn.color * diff;;
    //return float4(vertIn.normal, 1.0);
}