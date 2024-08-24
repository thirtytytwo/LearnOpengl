#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const std::string texturePath = "Textures/";
class Texture
{
public:
	unsigned int ID;
	Texture(std::string textureName)
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		//������ǲ�����������
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//����ͼƬ
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrChannels;
		std::string path = (texturePath + textureName + ".jpg");
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			//��������png��ʽ������ͨ��4
			if (nrChannels == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
	void Active(GLenum textureLocation)
	{
		glActiveTexture(textureLocation);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
	~Texture()
	{
		glDeleteTextures(1, &ID);
	}
};