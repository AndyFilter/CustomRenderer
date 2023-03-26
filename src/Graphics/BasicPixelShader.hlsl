struct PixelInput
{
    float4 position : SV_Position;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

PixelOutput main(PixelInput pixelInput)
{
    PixelOutput output;
    
    float2 uv = pixelInput.uv;
    float x = uv.x;
    float y = uv.y;
    
    float3 outColor;
    
    float2 z, c;

    // Mandelbrot Set
    c.x = 1.3333 * (x - 0.7) * 2.2;
    c.y = (y - 0.5) * 2.2;

    z = c;
    const int iter = 4;
    int i = 0;
    for (i = 0; i < iter; i++)
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if ((x * x + y * y) > 4.0)
            break;
        z.x = x;
        z.y = y;
    }

    output.attachment0.rg = float2(0,0);
    output.attachment0.b = float(i) / iter * uv.y;
    output.attachment0.r = float(i) / iter * uv.x;
    
    return output;
}