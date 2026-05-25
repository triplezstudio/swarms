module shader_interfaces;

public struct VSOutput
{
    public float4 pos : SV_Position;
    public float2 fragTexcoord;
    public uint textureId;
    public float4 color;
};

public struct PerObjectData
{
     public float4x4 model;
     public float4 color;
     public uint textureId;
     public uint pad0;
     public uint pad1;
     public uint pad2;
};
[[vk::binding(0,1)]]
public ConstantBuffer<PerObjectData> perObjectUBO;

public struct CameraData
{
    public float4x4 view;
    public float4x4 proj;

};
[[vk::binding(0,0)]]
public ConstantBuffer<CameraData> cameraUBO;



