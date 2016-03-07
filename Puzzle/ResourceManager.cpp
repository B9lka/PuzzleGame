#include <Windows.h>
#include <gdiplus.h>
#include "ResourceManager.hpp"

ResourceManagerClass::ResourceManagerClass()
{
}

ResourceManagerClass::~ResourceManagerClass()
{
	DestroyTextures();
}

void ResourceManagerClass::AddTexture(const WCHAR * filename)
{
	textures.push_back(TexturePair(filename, 0));
}

void ResourceManagerClass::LoadTextures()
{
	for (std::vector< TexturePair >::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		Gdiplus::Image* texture; 
	    texture = Gdiplus::Image::FromFile(it->first.c_str());
		
		it->second = new Texture;
		it->second->SetData(texture);
	}
}

void ResourceManagerClass::DestroyTextures()
{
	for (std::vector< TexturePair >::iterator it = textures.begin(); it != textures.end(); ++it)
		delete it->second;
	textures.clear();
}

Texture* ResourceManagerClass::GetTexture(int id) const
{
	return (size_t)id >= textures.size() || id < 0 ? 0 : textures[id].second;
}
