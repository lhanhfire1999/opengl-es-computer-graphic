#pragma once

#include <stdio.h>

class Tga
{
public:
	enum Status
	{
		Okay,
		CouldNotOpenFile,
		NotSupportIndexedColor,
		NotSupportCompressedFormat,
		InvalidBitsPerPixel
	};

private:
	unsigned short m_width, m_height;
	unsigned char m_bitsPerPixel, m_type;
	unsigned char* m_data;
	Status m_status;

public:
	Tga(const char* filePath) : m_data(NULL)
	{
		FILE* file = fopen(filePath, "rb");
		if (!file) { m_status = CouldNotOpenFile; return; }

		loadHeader(file);

		if (m_type == 1) { m_status = NotSupportIndexedColor; goto end; }
		if (m_type != 2 && m_type != 3) { m_status = NotSupportCompressedFormat; goto end; }
		if (m_bitsPerPixel != 24 && m_bitsPerPixel != 32) { m_status = InvalidBitsPerPixel; goto end; }

		loadBody(file);

		m_status = Okay;
		
		end:
		fclose(file);
	}

	~Tga()
	{
		if (m_data) delete[] m_data;
	}

	auto width() const { return m_width; }
	auto height() const { return m_height; }
	const unsigned char* data() const { return m_data; }
	auto okay() const { return m_status == Okay; }
	auto status() const { return m_status; }
	auto hasAlpha() const { return m_bitsPerPixel == 32; };

private:
	void loadHeader(FILE* file)
	{
		unsigned char cGarbage;
		short iGarbage;

		fread(&cGarbage, sizeof(cGarbage), 1, file);
		fread(&cGarbage, sizeof(cGarbage), 1, file);

		fread(&m_type, sizeof(m_type), 1, file);

		fread(&iGarbage, sizeof(iGarbage), 1, file);
		fread(&iGarbage, sizeof(iGarbage), 1, file);
		fread(&cGarbage, sizeof(cGarbage), 1, file);
		fread(&iGarbage, sizeof(iGarbage), 1, file);
		fread(&iGarbage, sizeof(iGarbage), 1, file);

		fread(&m_width, sizeof(m_width), 1, file);
		fread(&m_height, sizeof(m_height), 1, file);
		fread(&m_bitsPerPixel, sizeof(m_bitsPerPixel), 1, file);

		fread(&cGarbage, sizeof(cGarbage), 1, file);
	}

	void loadBody(FILE* file)
	{
		int channels = m_bitsPerPixel / 8;
		int total = m_width * m_height * channels;
		m_data = new unsigned char[total];

		fread(m_data, 1, total, file);

		// TGA stores data as BGR(A) so we have to swap R and B
		for (int i = 0; i < total; i += channels)
		{
			auto tmp = m_data[i];
			m_data[i] = m_data[i + 2];
			m_data[i + 2] = tmp;
		}
	}

};
