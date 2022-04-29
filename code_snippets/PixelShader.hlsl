
cbuffer cb
{
	float4 color[6];
};

//takes the color and returns the color with semantic for target position (it's a SYSTEM VALUE)
float4 main(uint ID : SV_PRIMITIVEID) : SV_TARGET
{
	return color[ID / 2];
}
