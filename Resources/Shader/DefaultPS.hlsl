
struct VSOutput
{
	float4 posProj : SV_POSITION;
	float4 color : COLOR;
};


float4 PS(VSOutput input) : SV_Target
{


	return input.color;
}