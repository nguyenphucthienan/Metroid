#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "define.h"
#include <windows.h>

class Graphics
{
public:
	Graphics(HINSTANCE hInstance, LPWSTR name, int width, int height, int fps, int isFullScreen);
	~Graphics();

	void initWindow();

	int isFullScreen();
	int getWidth();
	int getHeight();
	int getFrameRate();
	HWND getWnd();
	HINSTANCE gethInstance();

protected:
	HINSTANCE _hInstance;
	HWND _hWnd;

	LPWSTR _wcName;
	int _isFullScreen;

	int _width;
	int _height;
	int _fps;

	static HRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif // !__GRAPHICS_H__
