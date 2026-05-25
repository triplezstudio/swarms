module shader_interfaces;

public struct VSInputPosNormal
{
    public float3 inPosition;
    public float3 inNormal;

};

public struct VSOutput
{
    public float4 pos : SV_Position;
    public float2 fragTexcoord;
    public uint textureId;
    public float4 color;
};

public struct VSOutputLit
{
    public float4 pos : SV_Position;
    public float2 fragTexcoord;
    public uint textureId;
    public float4 color;
    public float3 normal;
};

public struct PerInstanceData
{
    public float4x4 model;
    public float4 color;
    public uint textureId;
    public uint pad0;
    public uint pad1;
    public uint pad2;
}
[[vk::binding(0,1)]]
public StructuredBuffer<PerInstanceData> instanceBuffer;

public struct CameraData
{
    public float4x4 view;
    public float4x4 proj;

};
[[vk::binding(0,0)]]
public ConstantBuffer<CameraData> cameraUBO;



