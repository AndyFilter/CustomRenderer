#pragma once

#ifndef SRC_GUI_GUI_H
#define SRC_GUI_GUI_H

#include <d3d11.h>

namespace GUI
{
	HWND Setup(HINSTANCE instance, int (*OnGuiFunc)());
	int DrawGui() noexcept;
	void Destroy() noexcept;
}

#endif // !SRC_GUI_GUI_H