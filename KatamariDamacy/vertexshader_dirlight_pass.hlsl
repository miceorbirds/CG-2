#pragma pack_matrix( row_major )

struct VS_OUTPUT
{
    float4 PositionClipSpace : SV_POSITION;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
    // Set up output
    VS_OUTPUT output;

	// Calculate the UV (0,0) to (2,2) via the ID
    float2 uv = float2(
		(id << 1) & 2, // id % 2 * 2
		id & 2);

	// Adjust the position based on the UV
    output.Position = float4(uv, 0, 1);
    output.Position.x = output.Position.x * 2 - 1;
    output.Position.y = output.Position.y * -2 + 1;

    return output;
}