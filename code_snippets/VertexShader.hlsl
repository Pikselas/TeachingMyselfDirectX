//the input and output need to be labeled by semantics

struct VertexShaderOut // this is the ouput type
{
	float4 color : COLOR; // this is the user defined semantic for color which will be passed to pixel shader
	float4 pos : SV_POSITION; // this is the SYSTEM_VALUE SEMANTIC this is fixed (and defined by the API) 
};

//cbuffer for constant buffer
cbuffer cbf
{
	row_major matrix transform; // the matrix (in cpu side the matrix is in row order but in gpu it's in coloumn order so fixinf it)
};

// main is the entry point and it takes 2 parameters
// first one is position and second one is color
// "POSITION" and "COLOR" is the semantic name which will be used to input the data from 
// our cpu 
// So our main returns a color and a position (for which every color that is returned with will be drawn) 
VertexShaderOut main(float3 pos : POSITION , float4 col : COLOR)
{
	VertexShaderOut Out;
	Out.pos = mul(float4(pos , 1.0f) , transform);
	Out.color = col;
	return Out;
}