#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <vector>
#include <string>
#include "Singleton.hpp"
#include "Texture.hpp"

class ResourceManagerClass
{
private:
	typedef std::pair<std::wstring, Texture*> TexturePair;
	std::vector< TexturePair > textures;

public:
	ResourceManagerClass();
	virtual ~ResourceManagerClass();

	void AddTexture(const WCHAR * filename);
	void LoadTextures();
	void DestroyTextures();

	Texture* GetTexture(int id) const;

};

typedef Singleton<ResourceManagerClass> ResourceManager;

#endif
