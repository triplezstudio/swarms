import shader_interfaces;


[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return vertIn.color;
}