struct VSOutput
{
    float4 pos : SV_Position;
};



[shader("fragment")]
float4 main(VSOutput vertIn) : SV_TARGET
{
    return float4(1, 1, 1, 1.0);
}