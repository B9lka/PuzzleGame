#ifndef TEXTURE_H
#define TEXTURE_H
#define GDIPVER     0x0110  // Use more advanced GDI+ features
#include <Windows.h>
#include <gdiplus.h>

class Texture
{
private:
	Gdiplus::Image* texture;

public:
	Texture()
	{
	}
		
	virtual ~Texture()
	{
		if (texture)
			delete texture;
	}
		
	float GetWidth() const
	{
		return (float)texture->GetWidth();
	}
	
	float GetHeight() const
	{
		return (float)texture->GetHeight();
	}

	const void* GetData() const
	{
		return (void *)texture;
	}

	void SetData(void* data)
	{
		texture =(Gdiplus::Image*) data;
	}
};
#endif