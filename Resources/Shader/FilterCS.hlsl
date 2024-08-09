
RWTexture2D<float4> t_filter : register(u0);
Texture2D t_test : register(t0);

[numthreads(32, 32, 1)]
void CS( uint3 dtID : SV_DispatchThreadID )
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	t_filter[dtID.xy] = color;
}