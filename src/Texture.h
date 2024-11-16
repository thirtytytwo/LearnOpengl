#pragma once

#include "stb_image.h"

using namespace std;
const std::string texturePath = "Assets/Textures/";

enum TextureType
{
	Default,
	Cube
};

class Texture
{
public:
	unsigned int ID;
	
	Texture(const std::string& path, TextureType _type = Default);

	void Active(GLenum textureLocation);
	void Bind();
	
private:
	TextureType type;
};

inline Texture::Texture(const std::string& path, TextureType _type): type(_type)
{
	glGenTextures(1, &ID);
	int width, height, nrChannels;
	unsigned char* data;
	string filePath;
	switch (_type)
	{
		case Default:
			filePath = (texturePath + path);
			data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum format = 0;
				if (nrChannels == 1)
					format = GL_RED;
				else if (nrChannels == 3)
					format = GL_RGB;
				else if (nrChannels == 4)
					format = GL_RGBA; 

				glBindTexture(GL_TEXTURE_2D, ID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				std::cout << "Failed to load texture" + filePath << std::endl;
				stbi_image_free(data);
			}
			break;
		case Cube:
			string cubeMapPaths[6] = { "right", "left", "top", "bottom", "front", "back" };
			for (int i = 0; i < 6; i++)
			{
				filePath = (texturePath + path + "/" + cubeMapPaths[i] + ".jpg");
				data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Failed to load texture" + filePath << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			break;
			
	}
}

inline void Texture::Active(GLenum textureLocation)
{
	glActiveTexture(textureLocation);
}
inline void Texture::Bind()
{
	switch (this->type)
	{
		case Default:
			glBindTexture(GL_TEXTURE_2D, ID);
			break;
		case Cube:
			glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
			break;
	}
}



