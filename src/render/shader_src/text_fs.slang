import shader_interfaces;

[[vk::binding(0,2)]]
Sampler2D textureSampler[];

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    float r = textureSampler[vertIn.textureId].Sample(vertIn.fragTexcoord).r;
    return float4(vertIn.color.r, vertIn.color.g, vertIn.color.b, r);

}