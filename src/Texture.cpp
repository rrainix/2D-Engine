
#include"Texture.h"
#include "Utils/Debug.h"

namespace graphics {

	Texture::Texture(const char* image, GLuint slot, GLenum format, GLenum pixelType)
	{
		load(image, slot, format, pixelType, 0);
	}

	Texture::~Texture() {
		Delete();
	}

	void Texture::load(const char* image, GLuint slot, GLenum format, GLenum pixelType, int filterMode) {
		std::string fullpath = RESOURCE_TEXTURE_PATH + image;
		int w, h, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load(fullpath.c_str(), &w, &h, &channels, 4);
		channels = 4; // jetzt sicher RGBA

		glGenTextures(1, &ID);
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, ID);

		// 1) Keine Zeilen-Padding-Fallen mehr
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// 2) Filter-Modus wie gehabt
		if (filterMode == 0) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if (filterMode == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// 3) Clamp-to-edge für saubere Ränder (9-Slice, UI, etc.)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// 4) Upload als RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, pixelType, bytes);

		// 5) Nur MipMaps erzeugen, wenn Du sie nutzt
		if (filterMode == 2)
			glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_size = glm::vec2{ (float)w, (float)h };
	}

	void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
	{
		GLuint texUni = glGetUniformLocation(shader.ID, uniform);
		// Shader needs to be activated before changing the value of a uniform
		shader.Activate();
		// Sets the value of the uniform
		glUniform1i(texUni, unit);
	}

	void Texture::Bind()
	{
		glActiveTexture(GL_TEXTURE0/* + unit*/);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	void Texture::Delete() const { glDeleteTextures(1, &ID); }

	void Texture::BindArray(Shader& shader, Texture textureArray[]) {
		GLuint textureArrayID;
		glGenTextures(1, &textureArrayID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

		// Set your dimensions (ensure that all your textures are the same size)
		int WIDTH = 16;
		int HEIGHT = 16;
		int numLayers = 2;  // Two textures in the array

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, WIDTH, HEIGHT, numLayers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Loop through each layer, load the image, and upload it
		for (int layer = 0; layer < numLayers; ++layer) {
			// Get the file path from your texture object.
			// Here we assume that your Graphics::Texture has a method getPath() that returns the file path.
			std::string path = textureArray[layer].path();

			int imgWidth, imgHeight, nrChannels;
			// Optionally flip the image vertically if needed:
			stbi_set_flip_vertically_on_load(true);

			// Load the image. Forcing 4 channels (RGBA) regardless of original image channels.
			unsigned char* data = stbi_load(path.c_str(), &imgWidth, &imgHeight, &nrChannels, 4);
			if (data) {
				// It's a good idea to verify the image dimensions match your texture array size.
				if (imgWidth != WIDTH || imgHeight != HEIGHT) {
					std::cerr << "Image dimensions (" << imgWidth << "x" << imgHeight
						<< ") do not match expected dimensions (" << WIDTH << "x" << HEIGHT << ") for " << path << std::endl;
				}
				// Upload the image data to the corresponding layer
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
					0, 0, layer,      // xoffset, yoffset, layer (zoffset)
					WIDTH, HEIGHT, 1,  // width, height, depth (1 layer)
					GL_RGBA, GL_UNSIGNED_BYTE, data);

				// Free the image memory when done
				stbi_image_free(data);
			}
			else {
				std::cerr << "Failed to load image: " << path << std::endl;
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
	}
}
