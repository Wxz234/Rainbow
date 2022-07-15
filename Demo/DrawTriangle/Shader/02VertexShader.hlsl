struct VS_OUTPUT {
	float4 POSITION: SV_POSITION;
	float2 TEX: TEXCOORD;
};

VS_OUTPUT main(uint VertexID : SV_VertexID) {
	VS_OUTPUT Out;
	Out.TEX = float2((VertexID << 1) & 2, VertexID & 2);
	Out.POSITION = float4(Out.TEX * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	return Out;
}