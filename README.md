# Custom Renderer
This projects simply explains how to setup and use **DirectX 11 with ImGui**.\
It's mosty aimed for new people with no **prior experience with** ImGui + **D3D11** combo.

## How setup the Project
So, to start you will need to:
- Download all the necessary dependencies which are: [**DirectX SDK**](https://www.microsoft.com/en-us/download/details.aspx?id=6812), [ImGui](https://github.com/ocornut/imgui) (only if you feel like you ned the lastes version),
- Download the source code (there is no release binary becasue it would be pointless),
- <details open style="margin:0"><summary>Link all the dependencies</summary><ol type = "1">	<li> Project Settings &rarr; Linker &rarr; Input &rarr; Additional Dependencies &rarr; add <i>d3dx11.lib;d3d11.lib;</i></li>	<li> Project Settings &rarr; Linker &rarr; System &rarr; SubSystem &rarr; set to <i>Windows (/SUBSYSTEM:WINDOWS)</i></li>	<li> (Set Platform to x64) &rarr; Project Settings &rarr; Configuration Properties &rarr; VC++ Directories &rarr; Include Directories &rarr; set to <i>$(IncludePath);$(DXSDK_DIR)Include</i> </li> <li> Project Settings &rarr; Configuration Properties &rarr; VC++ Directories &rarr; Library Directories &rarr; set to <i>$(LibraryPath);$(DXSDK_DIR)Lib\x86</i> </li>	</details>
- If you have downloaded the latest version of ImGui, update the files located in *..\src\External\ImGui* with the new files.

**Congratulations!** If you are feeling brave you might try building the code.
> ###### I probablly forgot about something and you are going to get 1000 linker errors..

## Additional Info (things to keep in mind)
The shaders are there just to give you an example on how to set them up.\
If you actually want to render some 2D / 3D things you will have to update the *vertex buffer* (probablly) every frame.

### This is a beginner friendly project so here are some important tips about DX11:
- What Vertex Shader returs generally goes to the Pixel Shader so you might see something like this:\
\
    *BasicVertexShader.hlsl*
    ``` GLSL
    struct VertexOutput // Pixel Shader Input
    {
        float4 position : SV_Position;
        float3 color : COLOR0;
        float2 uv : TEXCOORD0;
    };
    ```
    *BasicPixelShader.hlsl*
    ``` GLSL
    struct PixelInput
    {
        float4 position : SV_Position;
        float3 color : COLOR0;
        float2 uv : TEXCOORD0;
    };
    ```
- With the flag [*D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST*](https://github.com/AndyFilter/CustomRenderer/main/src/Graphics/Graphics.cpp#L156) set, it seems that You can only draw triangles. That's why drawing a rectagle takes 6 vertices rather than 4.
- Good sources for learning:
   - [Raw DirectX 11](https://alain.xyz/blog/raw-directx11)
   - [Pixel Shader Editor](http://pixelshaders.com/editor/)
   - [Microsoft Samples](https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld)
   - [MS docs on shaders](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics)
   - [Great Youtube Playlist about DX11](https://youtube.com/playlist?list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2)