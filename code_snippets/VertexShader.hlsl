

//the input and output need to be labeled by semantics

struct VertexShaderOut // this is the ouput type
{
	float4 color : COLOR; // this is the user defined semantic for color which will be passed to pixel shader
	float4 pos : SV_POSITION; // this is the SYSTEM_VALUE SEMANTIC this is fixed (and defined by the API) 
};


// main is the entry point and it takes 2 parameters
// first one is position and second one is color
// "POSITION" and "COLOR" is the semantic name which will be used to input the data from 
// our cpu 
// So our main returns a color and a position (for which every color that is returned with will be drawn) 
VertexShaderOut main(float3 pos : POSITION , float4 col : COLOR)
{
	VertexShaderOut Out;
	Out.pos = float4(pos , 1.0f) ;
	Out.color = col;
	return Out;
}