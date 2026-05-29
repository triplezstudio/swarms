import shader_interfaces;

[[vk::binding(0,2)]]
Sampler2D textureSampler[];

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    float4 textureColor = textureSampler[vertIn.textureId].Sample(vertIn.fragTexcoord);
    float4 finalColor = textureColor * vertIn.color;
    return finalColor;
}