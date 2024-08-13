
struct VSOutput
{
	float4 posProj : SV_POSITION;
	float3 posWorld : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};


float4 PS(VSOutput input) : SV_Target
{


	return float4(1.0, 1.0, 1.0, 1.0);
}