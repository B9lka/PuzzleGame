#include "ResourceManager.hpp"
#include "GDIWorker.hpp"

ULONG_PTR GDIWorker::gdiplusToken;
GDIWorker::GDIWorker(HWND hWnd) : m_hWnd(hWnd)
{
	GetClientRect(m_hWnd, &m_size);
	m_hDC = GetDC(m_hWnd);
	m_graphics = new Gdiplus::Graphics(m_hDC);
	m_buffer = new Gdiplus::Bitmap(m_size.right, m_size.bottom);
	m_temp = new Gdiplus::Graphics(m_buffer);
}

GDIWorker::~GDIWorker()
{
	m_fonts.clear();
	delete(m_graphics);
	delete(m_temp);
	delete(m_buffer);
	ReleaseDC(m_hWnd, m_hDC);
}

void GDIWorker::Init()
{
	Gdiplus::GdiplusStartupInput gdipludStartupToken;
	GdiplusStartup(&gdiplusToken, &gdipludStartupToken, NULL);
}

void GDIWorker::Destroy()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

void GDIWorker::Redraw()
{
	m_graphics->DrawImage(m_buffer, 0, 0, m_size.right, m_size.bottom);
}

void GDIWorker::DrawTexture(const Rect& screenCoords, int textureId, const Rect& textureCoord)
{
	Gdiplus::Image* texture = (Gdiplus::Image*) ResourceManager::GetInstance().GetTexture(textureId)->GetData();
	if (!texture)
		return;

	Gdiplus::RectF screenCoordsWrap;
	screenCoordsWrap.X = screenCoords.left;
	screenCoordsWrap.Y = screenCoords.top;
	screenCoordsWrap.Width = screenCoords.right;
	screenCoordsWrap.Height = screenCoords.bottom;
	
	Gdiplus::RectF textureCoordsWrap;
	textureCoordsWrap.Height = textureCoord.bottom;
	textureCoordsWrap.X = textureCoord.left;
	textureCoordsWrap.Y = textureCoord.top;
	textureCoordsWrap.Width = textureCoord.right;
	//int widthIn = textureCoord.right > textureCoord.bottom ? textureCoord.bottom : textureCoord.right;
	//m_temp->DrawImage(texture, screenCoordsWrap,((float)widthIn), ((float)widthIn), ((float)widthIn), ((float)widthIn), Gdiplus::UnitPixel);
	m_temp->DrawImage(texture, screenCoordsWrap, textureCoordsWrap,Gdiplus::UnitPixel);
}

void GDIWorker::DrawTexture(const Rect& screenCoords, int textureId)
{
	Texture* texture = ResourceManager::GetInstance().GetTexture(textureId);
	if (!texture)
		return;
	
	Rect textureCoords;
	textureCoords.left = 0.0f;
	textureCoords.top = 0.0f;
	textureCoords.right = texture->GetWidth();
	textureCoords.bottom = texture->GetHeight();
	
	GDIWorker::DrawTexture(screenCoords, textureId,textureCoords);
}

void GDIWorker::BeginScene()
{
	BeginPaint(m_hWnd, &m_ps);
}

void GDIWorker::EndScene()
{
	m_graphics->DrawImage(m_buffer, 0, 0, m_size.right, m_size.bottom);
	EndPaint(m_hWnd, &m_ps);
}

void GDIWorker::FontCreate(WCHAR * fontName, Gdiplus::REAL size, int style, Gdiplus::Unit unit)
{
	m_fonts.push_back(new Gdiplus::Font(fontName, size, style, unit));
}

void GDIWorker::DrawString(char * string, int fontIndex, Gdiplus::PointF coords, Gdiplus::Color color, Gdiplus::StringFormat * format)
{
	int len = strlen(string);
	WCHAR* buffer = new WCHAR[len + 1];
	buffer[len] = 0;
	
	//mbstowcs(buffer, string, len);

	Gdiplus::SolidBrush brush(color);
	if (format != NULL)
		m_temp->DrawString(buffer, len, m_fonts[fontIndex], coords, format, &brush);
	else
		m_temp->DrawString(buffer, len, m_fonts[fontIndex], coords, &brush);

	delete(buffer);
}

void GDIWorker::DrawImage(Gdiplus::Image * picture, Gdiplus::RectF rect)
{
	m_temp->DrawImage(picture, rect);
}

void GDIWorker::DrawImage(HBITMAP picture, Gdiplus::RectF rect)
{
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(picture, NULL);
	m_temp->DrawImage(bitmap, rect);
	delete(bitmap);
}

void GDIWorker::Clear(Gdiplus::Color color)
{
	m_temp->Clear(color);  
}
