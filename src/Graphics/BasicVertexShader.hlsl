struct VertexInput
{
    float3 inPos : POSITION;
    float3 inColor : COLOR;
    float2 uv : TEXCOORD;
};

struct VertexOutput // Pixel Shader Input
{
    float4 position : SV_Position;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VertexOutput main(VertexInput vertexInput)
{
    float3 inColor = vertexInput.inColor;
    float3 inPos = vertexInput.inPos;
    float3 outColor = inColor;

    VertexOutput output;
    output.position = float4(inPos, 1.f);
    output.color = outColor;
    output.uv = vertexInput.uv;
    return output;
}