cbuffer CBuf
{
    matrix model;
    matrix modelViewProj;
};

float4 main(float3 pos : Position, float3 n : Normal) : SV_POSITION
{
    return mul(float4(pos, 1.0f), modelViewProj);
}