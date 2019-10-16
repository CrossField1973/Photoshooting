cbuffer lightBuffer
{
	float3 lightPos;
};

static const float3 materalColor = { 1.0f, 1.0f, 1.0f };
static const float3 ambient = { 0.3f, 0.3f, 0.3f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.065f;
static const float attQuad = 0.0075f;


float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_TARGET
{
	//fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	//diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	//diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	//final color
    return float4(saturate(diffuse + ambient) * materalColor, 1.0f);

}