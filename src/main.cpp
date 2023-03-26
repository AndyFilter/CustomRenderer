#include "GUI/gui.h"
#include "External/ImGui/imgui.h"
#include "Graphics/Graphics.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

int OnGui()
{
	// Display current framerate
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ char* cmdLine, _In_ int showCmd)
{
	// Gui sets-up GRAPHICS
	GUI::Setup(instance, OnGui);

	ImGui::GetIO().IniFilename = nullptr;

	while (true)
	{
		if (GRAPHICS::RenderFrame())
			break;
	}

	GUI::Destroy();
	GRAPHICS::Destroy();

	exit(0);
}
