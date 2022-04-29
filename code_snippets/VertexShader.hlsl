//cbuffer for constant buffer
cbuffer cbf
{
   matrix transform; // the matrix (in cpu side the matrix is in row order but in gpu it's in coloumn order so fixinf it)
};


float4 main(float3 pos : POSITION) : SV_POSITION
{
	return mul(float4(pos , 1.0f) , transform);
}