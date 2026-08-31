#pragma once

#include "Maths/Alias.h"

#include <string>

using std::string;

namespace Leviathan
{
	class Texture
	{
	private:
		string m_fileName;

		int32 m_width;
		int32 m_height;
		int32 m_channels;

		uint32 m_handle;

		uint8* m_pixels;
		bool m_stbiLoaded;

	public:
		Texture();
		explicit Texture(string fileName);
		explicit Texture(int32 w, int32 h, uint8* pixels = nullptr);
		~Texture();

	public:
		uint32 Handle() const;

	private:
		void Load();

	};
}
