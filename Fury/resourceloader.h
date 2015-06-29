#pragma once
#include <fstream>
class ResourceLoader
{
public:
	static std::string* loadFileText(const std::string& path, bool binary = false)
	{
		std::fstream file(path, std::ios::in | std::ios::ate | (binary ? std::ios::binary : 0));
		if (!file)
			return NULL;

		std::string* out = new std::string();
		out->resize((unsigned)file.tellg());
		file.seekg(0);
		file.read((char*)(&(*out)[0]), out->size());
		return out;
	}

	static std::string pathToFileName(const std::string& path)
	{
		size_t lastSlashLoc = path.size() - 1;
		while (path[lastSlashLoc] != '\\' && path[lastSlashLoc] != '/' && lastSlashLoc > 0)
			--lastSlashLoc;

		return path.substr(lastSlashLoc + 1, path.size() - lastSlashLoc + 1);
	}

	static std::string getFileExtension(const std::string& path)
	{
		size_t periodLoc = path.size() - 1;
		while (path[periodLoc] != '.' && periodLoc > 0)
			--periodLoc;
		return path.substr(periodLoc + 1, path.size() - periodLoc + 1);
	}
};