cbuffer lightBuffer
{
	float3 lightPos;
};

static const float3 materalColor = { 1.0f, 1.0f, 1.0f };
static const float3 ambient = { 0.3f, 0.3f, 0.3f };
static const float3 diffuseColor = { 1.0f, 0.0f, 0.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.065f;
static const float attQuad = 0.0075f;
static const float specularIntensity = 0.6f;
static const float specularPower = 30.0f;


float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_TARGET
{
	//fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	//attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	//diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	//reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    //calculate specular intensita based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    //final color
    return float4(saturate(diffuse + ambient + specular) * materalColor, 1.0f);

}