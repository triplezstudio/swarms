struct VSOutput
{
    float4 pos : SV_Position;
    float2 fragTexcoord;
    uint textureId;
};


[[vk::binding(0,2)]]
Sampler2D textureSampler[];

[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return textureSampler[vertIn.textureId].Sample(vertIn.fragTexcoord);
}