struct VS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    int index : INDEX;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    int index : INDEX;
};



VS_OUT main(VS_IN input)
{
    VS_OUT output;

    output.pos = input.pos;
    output.uv = input.uv;
    output.index = input.index;

    return output;
}