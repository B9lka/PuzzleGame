#ifndef GDIWORKER_H
#define GDIWORKER_H

#include <vector>
#include <Windows.h>
#include <gdiplus.h>
#include "MiniGameInterface.hpp"

#pragma comment (lib, "GDIPlus.lib")

class GDIWorker
{
public:
	GDIWorker(HWND hWnd);
	~GDIWorker();
	static void Init();
	static void Destroy();
	
	void Redraw();
	void Clear(Gdiplus::Color color);
	
	void DrawTexture(const Rect& screenCoords, int textureId, const Rect& textureCoord);
	void DrawTexture(const Rect& screenCoords, int textureId);

	void BeginScene();
	void EndScene();

	void FontCreate(WCHAR* fontName, Gdiplus::REAL size, int style = Gdiplus::FontStyleRegular, Gdiplus::Unit unit = Gdiplus::UnitPoint);
	void DrawString(char* string, int fontIndex, Gdiplus::PointF coords, Gdiplus::Color color, Gdiplus::StringFormat* format = NULL);
	void DrawImage(Gdiplus::Image* picture, Gdiplus::RectF rect);
	void DrawImage(HBITMAP picture, Gdiplus::RectF rect);

private:
	Gdiplus::Graphics* m_graphics;
	Gdiplus::Graphics* m_temp;
	Gdiplus::Bitmap* m_buffer;
	
	static ULONG_PTR gdiplusToken;
	
	HDC m_hDC;
	HWND m_hWnd;
	PAINTSTRUCT m_ps;
	
	RECT m_size;

	std::vector<Gdiplus::Font*> m_fonts;
};
#endif
