#pragma once
#include "stdafx.h"
#include <GL/glew.h>
#ifdef _WIN32
#include <Windows.h>
#else
#endif
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <limits>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "UI.hpp"
#include <assimp/material.h>
#include "nv_dds.h"

namespace TDModelView
{
	struct Shader
	{
		unsigned int ID = 0;
		std::string handle = "";
		void clear() { ID = 0; handle = ""; }
		void use() { glUseProgram(ID); }
		void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
		void setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
		void setUint(const std::string& name, int value) const { glUniform1ui(glGetUniformLocation(ID, name.c_str()), value); }
		void setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
		void setIvec2(const std::string& name, const glm::ivec2& value) const { glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setIvec3(const std::string& name, const glm::ivec3& value) const { glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setIvec4(const std::string& name, const glm::ivec4& value) const { glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setVec2(const std::string& name, const glm::vec2& value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setVec2(const std::string& name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
		void setVec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setVec3(const std::string& name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
		void setVec4(const std::string& name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
		void setVec4(const std::string& name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
		void setMat2(const std::string& name, const glm::mat2& mat) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
		void setMat3(const std::string& name, const glm::mat3& mat) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
		void setMat4(const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
		void checkCompileErrors(GLuint shader, std::string handle) {
			GLint success;
			GLchar infoLog[1024];
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::string str = ("ERROR::PROGRAM_LINKING_ERROR of type: PROGRAM\n" + std::string(infoLog)
					+ "\n" + handle);
				ErrorMessageBox(str);
			}
		}
	};
	struct Texture
	{
		GLuint id = 0;
		std::string filepath = "";
		Texture(){}
		~Texture(){}

		Texture(cv::Mat& img, std::string filename)
		{
			this->filepath = filename;
			if (img.channels() != 4)
				cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
			int h = img.rows;
			int w = img.cols;
			unsigned int sz = img.total() * img.elemSize();
			uint8_t* data = new uint8_t[sz];
			memcpy(data, img.data, sz);
			img.deallocate();
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			if (this->id)
				glDeleteTextures(1, &this->id);
			glGenTextures(1, &this->id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void loadDDS(std::string path) {
			nv_dds::CDDSImage image;
			nv_dds::CSurface surf;

			try
			{
				bool flipImage = false;
				image.load(path, flipImage);
			}
			catch (std::exception e1)
			{
				ErrorMessageBox(e1.what());
				return;
			}

			int w = image.get_width();
			int h = image.get_height();
			int d = image.get_depth();
			int type = -1;
			GLuint totalBytes = image.get_size();
			GLuint internalFormat = image.get_format();
			if (image.get_num_mipmaps() > 1) {
				surf = image.get_mipmap(0);
				w = surf.get_width();
				h = surf.get_height();
				d = surf.get_depth();
				totalBytes = surf.get_size();
			}

			if (image.get_type() == nv_dds::TextureType::TextureFlat)
				type = GL_TEXTURE_2D;

			// Convert 'nv_dds' formatting from using swapped channel formats to something OpenGL can handle.
			switch (internalFormat) {
			case GL_BGRA_EXT:
				internalFormat = GL_RGBA;
				break;
			case GL_BGR_EXT:
				internalFormat = GL_RGB;
				break;
			case GL_LUMINANCE:
				internalFormat = GL_RED;
				break;
			}

			int format = internalFormat;
			switch (d) {
			case 1:
				format = GL_RED;
				break;
			case 2:
				ErrorMessageBox("ERROR! Cannot load 2 channel .dds files.");
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			};

			bool isCompressed = false;			
			switch (internalFormat) {
			case GL_COMPRESSED_RED:
			case GL_COMPRESSED_RG:
			case GL_COMPRESSED_RGB:
			case GL_COMPRESSED_RGBA:
			case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			case GL_COMPRESSED_RED_RGTC1_EXT:
			case GL_COMPRESSED_SIGNED_RED_RGTC1_EXT:
			case GL_COMPRESSED_RED_GREEN_RGTC2_EXT:
			case GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT:
			case GL_COMPRESSED_RGB_FXT1_3DFX:
			case GL_COMPRESSED_RGBA_FXT1_3DFX:
			case GL_COMPRESSED_RGB8_ETC2:
			case GL_COMPRESSED_SRGB8_ETC2:
			case GL_COMPRESSED_ALPHA:
			case GL_COMPRESSED_INTENSITY:
			case GL_COMPRESSED_LUMINANCE:
			case GL_COMPRESSED_SRGB_ALPHA:
			case GL_COMPRESSED_SRGB:
			case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
			case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
			case GL_COMPRESSED_RGBA_BPTC_UNORM:
			case GL_COMPRESSED_RGBA8_ETC2_EAC:
			case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
			case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT:
			case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
			case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
			case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
			case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
			case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
			case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
			case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
			case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
			case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
			case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
			case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
			case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
			case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
			case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
			case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
				isCompressed = true;
			}			

			int dataType = GL_UNSIGNED_BYTE;
			uint8_t* data = new uint8_t[totalBytes];
			unsigned int byte_counter = 0;
			switch (type)
			{
			case GL_TEXTURE_2D:
				if (image.get_num_mipmaps() > 1)
					memcpy(data, surf, surf.get_size());
				else
					memcpy(data, image, image.get_size());
				break;
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_3D:
			case -1:
				ErrorMessageBox("ERROR! Cannot load non-2D DDS files.");
			}

			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			if (this->id)
				glDeleteTextures(1, &this->id);
			glGenTextures(1, &this->id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (isCompressed)
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, totalBytes, &data[0]);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, dataType, &data[0]);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			delete[] data;
			image.clear();
		}

		Texture(std::string filename, std::string dir) {
			this->filepath = checkFilepath(filename, dir);
			std::string ext = getExtension(this->filepath);
			if (ext != ".dds" && !std::filesystem::is_regular_file(filepath)) {
				ErrorMessageBox("ERROR! Could not load texture " + filepath);
				return;
			}
			else if (ext == ".dds")
			{
				loadDDS(filepath);
				return;
			}

			cv::Mat img = cv::imread(filename, -1);
			GLenum format = GL_RGB;
			GLenum internalFormat = GL_RGB;
			GLenum dataType = GL_UNSIGNED_BYTE;
			if (img.empty())
				return;
			cv::flip(img, img, 0);
			switch (img.depth()) {
			case CV_8U:
				if (img.channels() == 1) {
					cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);
					format = internalFormat = GL_RGB;
				}
				else if (img.channels() == 3) {
					cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
					format = internalFormat = GL_RGB;
				}
				else if (img.channels() == 4) {
					cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
					format = internalFormat = GL_RGBA;
				}
				dataType = GL_UNSIGNED_BYTE;
				break;
			case CV_16U:
				if (img.channels() == 1) {
					cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB16;
				}
				else if (img.channels() == 3) {
					cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB16;
				}
				else if (img.channels() == 4) {
					cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
					format = GL_RGBA;
					internalFormat = GL_RGBA16;
				}
				dataType = GL_UNSIGNED_SHORT;
				break;
			case CV_16F:
				if (img.channels() == 1) {
					cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB16F;
				}
				else if (img.channels() == 3) {
					cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB16F;
				}
				else if (img.channels() == 4) {
					cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
					format = GL_RGBA;
					internalFormat = GL_RGBA16F;
				}
				dataType = GL_FLOAT;
				break;
			case CV_32F:
				if (img.channels() == 1) {
					cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB32F;
				}
				else if (img.channels() == 3) {
					cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
					format = GL_RGB;
					internalFormat = GL_RGB32F;
				}
				else if (img.channels() == 4) {
					cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
					format = GL_RGBA;
					internalFormat = GL_RGBA32F;
				}
				dataType = GL_FLOAT;
				break;
			}

			int h = img.rows;
			int w = img.cols;
			unsigned int sz = img.total() * img.elemSize();
			uint8_t* data = new uint8_t[sz];
			memcpy(data, img.data, sz);
			img.deallocate();
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			if (this->id)
				glDeleteTextures(1, &this->id);
			glGenTextures(1, &this->id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, dataType, &data[0]);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void clear() 
		{
			if (id)
				glDeleteTextures(1, &id);
			filepath = "";
		}
	};

	struct Material 
	{
		bool useBumpMap = false;
		float alphaCutoff = 0.001f;
		float bumpMultiplier = 1.0f;
		float displacementMapBias = 0.0f;
		bool parallaxCrop = false;
		int parallaxSamples = 10;
		float heightMultiplier = 1.0f;
		float emissiveMultiplier = 1.0f;
		float lightmapMultiplier = 1.0f;
		float ambientocclusionMultiplier = 0.1f;
		glm::vec3 ambient = glm::vec3(0);
		glm::vec3 diffuse = glm::vec3(1);
		glm::vec3 specular = glm::vec3(1);
		glm::vec3 emissive = glm::vec3(0);
		glm::vec3 transparent = glm::vec3(0);
		float opacity = 1.f;
		float roughness = 1.f;
		float shininessStrength = 1.f;
		float specularFactor = 1.f;
		float metalness = 0.0f;
		float ior = 1.5f;
		std::array<std::shared_ptr<Texture>, int(aiTextureType_UNKNOWN) + 1> textures;
		bool HasTexture(aiTextureType texType) { return (textures[texType] != nullptr); }
		void AddTexture(const std::shared_ptr<Texture>& spTexture, aiTextureType texType) { textures[texType] = spTexture; }
		void BindTexture(aiTextureType texType) {
			if (textures[texType] != nullptr)
				glBindTexture(GL_TEXTURE_2D, textures[texType]->id);
			else
				glBindTexture(GL_TEXTURE_2D, 0);
		}
		void setUniforms(Shader* prog, bool useModelNormals) {
			static const std::vector<std::string> materialUniformNamesNoSpace{
		   "None",
		   "Diffuse",
		   "Specular",
		   "Ambient",
		   "Emissive",
		   "Height",
		   "Normal",
		   "Shininess",
		   "Opacity",
		   "Displacement",
		   "Lightmap",
		   "Reflection",
		   "Albedo",
		   "NormalCamera",
		   "EmissiveColor",
		   "Metalness",
		   "Roughness",
		   "AmbientOcclusion",
		   "Unknown"//Often the metalRoughness map is detected as 'Unknown'
			};

			prog->setVec3("material.diffuse", diffuse);
			prog->setVec3("material.specular", specular);
			prog->setVec3("material.ambient", ambient);
			prog->setVec3("material.emissive", emissive);
			prog->setVec3("material.transparent", transparent);
			prog->setFloat("material.specularFactor", specularFactor);
			prog->setFloat("material.roughness", roughness);
			prog->setFloat("material.opacity", opacity);
			prog->setFloat("material.ior", ior);
			prog->setFloat("material.metalness", metalness);
			prog->setFloat("displacementMapAmplitude", bumpMultiplier);

			//static unsigned int Udiffuse = glGetUniformLocation(prog->ID, "material.diffuse");
			//glUniform3fv(Udiffuse, 1, &diffuse[0]);
			//static unsigned int Uspecular = glGetUniformLocation(prog->ID, "material.specular");
			//glUniform3fv(Udiffuse, 1, &specular[0]);
			//static unsigned int Uemissive = glGetUniformLocation(prog->ID, "material.emissive");
			//glUniform3fv(Uemissive, 1, &emissive[0]);
			//static unsigned int Utransparent = glGetUniformLocation(prog->ID, "material.transparent");
			//glUniform3fv(Utransparent, 1, &transparent[0]);
			//static unsigned int Uambient = glGetUniformLocation(prog->ID, "material.ambient");
			//glUniform3fv(Uambient, 1, &ambient[0]);
			//static unsigned int UspecularFactor = glGetUniformLocation(prog->ID, "material.specularFactor");
			//glUniform1f(UspecularFactor, specularFactor);
			//static unsigned int Uroughness = glGetUniformLocation(prog->ID, "material.roughness");
			//glUniform1f(Uroughness, roughness);
			//static unsigned int Uopacity = glGetUniformLocation(prog->ID, "material.opacity");
			//glUniform1f(Uopacity, opacity);
			//static unsigned int Uior = glGetUniformLocation(prog->ID, "material.ior");
			//glUniform1f(Uior, ior);
			//static unsigned int Umetalness = glGetUniformLocation(prog->ID, "material.metalness");
			//glUniform1f(Umetalness, metalness);
			//static unsigned int UbumpMultiplier = glGetUniformLocation(prog->ID, "material.bumpMultiplier");
			//glUniform1f(UbumpMultiplier, bumpMultiplier);

			prog->setFloat("displacementMapBias", displacementMapBias);
			prog->setInt("parallaxSamples", parallaxSamples);
			prog->setFloat("heightMapAmplitude", heightMultiplier);
			prog->setFloat("lightmapMultiplier", lightmapMultiplier);
			prog->setFloat("emissiveMapAmplitude", emissiveMultiplier);
			prog->setFloat("ambientocclusionMapAmplitude", ambientocclusionMultiplier);
			prog->setFloat("alphaCutoff", alphaCutoff);
			prog->setBool("hasDisplacementMap", HasTexture(aiTextureType_DISPLACEMENT));
			prog->setBool("useBumpMap", useBumpMap);
			for (int i = 1; i < aiTextureType_UNKNOWN; ++i)
			{
				if(i == (int)aiTextureType_NORMALS && useModelNormals)
					prog->setBool("hasNormalMap", false);
				else
					prog->setBool("has" + materialUniformNamesNoSpace[i] + "Map", HasTexture(aiTextureType(i)));
			}
		}
	};

	class TextureBank 
	{
	public:
		TextureBank() {}
		~TextureBank() {}
		void remove(int i) {
			if (i < 0 || i >= textures.size())
				return;
			this->remove(textures[i].filepath);
		}
		void remove(std::string filename);
		int add(std::string filename, std::string dir) {
			// Add only unique textures.
			if (exists(filename)) {
				Texture tx = get(filename);
				return tx.id;
			}
			else {
				Texture tx = Texture(filename, dir);
				if (tx.id != 0) {
					textures.push_back(tx);
					return tx.id;
				}
				else {
					return -1;
				}
			}
		}
		int add(Texture tx) {
			if (exists(tx.filepath))
				return tx.id;
			else {
				if (tx.id != 0) {
					textures.push_back(tx);
					return tx.id;
				}
				else {
					return -1;
				}
			}
		}
		void clear() {
			for (int i = 0; i < textures.size(); ++i) {
				if (textures[i].id)
					glDeleteTextures(1, &textures[i].id);
			}
			textures.clear();
		}
		bool exists(std::string filename) {
			for (int i = 0; i < textures.size(); ++i) {
				if (textures[i].filepath == filename)
					return true;
			}
			return false;
		}
		Texture get(std::string filename) {
			for (int i = 0; i < textures.size(); ++i) {
				if (textures[i].filepath == filename)
					return textures[i];
			}
			return Texture();
		}
		inline std::shared_ptr<Texture> getPtr(std::string filename) {
			for (int i = 0; i < textures.size(); ++i) {
				if (textures[i].filepath == filename)
					return std::make_shared<Texture>(textures[i]);
			}
			return nullptr;
		}
		Texture get(int idx) {
			if (idx > 0 && idx < textures.size())
				return textures[idx];
			return Texture();
		}
		Texture GenIrradianceMap(Texture tx) {

		}
	private:
		Shader* irradianceShader = nullptr;
		std::vector<Texture> textures;
	};

    struct Vertex 
	{
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 uv = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec3 tangent = glm::vec3(0.0f);
        glm::vec3 bitangent = glm::vec3(0.0f);
        void Orthonormalize() {
            normal = glm::normalize(normal);
            tangent = glm::orthonormalize(tangent, normal);
            if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f)
                tangent = tangent * -1.0f;            
        }
    };

    struct BoundingBox 
	{
        glm::vec3 bboxMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 bboxMax = glm::vec3(-std::numeric_limits<float>::max());
        glm::vec3 center(){return (bboxMax + bboxMin)*0.5f;}
        glm::vec3 extent(){return (bboxMax - bboxMin)*0.5f;}
		float minDim() { glm::vec3 ext = extent(); return glm::min(glm::min(ext.x,ext.y), ext.z); }
		float maxDim() { glm::vec3 ext = extent(); return glm::max(glm::max(ext.x,ext.y), ext.z); }
        void Reset(){
            bboxMin = glm::vec3(std::numeric_limits<float>::max());
            bboxMax = glm::vec3(-std::numeric_limits<float>::max());
        }
    };

    struct Camera 
	{
        const glm::vec3 WorldFront = glm::vec3(0, 0, 1);
        const glm::vec3 WorldRight = glm::vec3(1, 0, 0);
        const glm::vec3 WorldUp = glm::vec3(0, 1, 0);
        glm::vec3 angles = glm::vec3(0.0);
        glm::vec3 front = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(glm::vec3(0));
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        float movementSpeed = 0.2f;
        float aspect = 0.0f;
        float angle = 0.0f;
        const float fov = 45.0f;// 45 deg
        const float fov_rad = 0.7853981634f;// 45 deg * (PI/180)
        const float zNear = 0.5f;
        const float zFar = 5000.0f;
        glm::mat4 VP = glm::mat4(1.0f);
        void Rotate(glm::vec3 angle){
            // Convert camera values to range [0,2*PI]
            static const float pi_val = 6.2831853071795864f;
            while (angle.x < 0)
                angle.x += pi_val;
            while (angle.y < 0)
                angle.y += pi_val;
            while (angle.z < 0)
                angle.z += pi_val;

            while (angle.x > pi_val)
                angle.x -= pi_val;
            while (angle.y > pi_val)
                angle.y -= pi_val;
            while (angle.z > pi_val)
                angle.z -= pi_val;

            this->angles = angle;
            glm::quat rotationX = glm::angleAxis(angles.x, WorldRight);
            glm::quat rotationY = glm::angleAxis(angles.y, WorldUp);
            glm::quat rotationZ = glm::angleAxis(angles.z, WorldFront);
            rotation = glm::normalize(rotationZ * rotationX * rotationY);
            Update();
        }
		void Update();
    };

    class Mesh 
	{
    public:
        BoundingBox bbox;//bounds of mesh without any transformation
        GLuint numIndices = 0;
        GLuint numVertices = 0;
        std::shared_ptr<Material> material = nullptr;
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        GLuint EBO = 0;
        bool loaded = false;
        GLuint VBO = 0;
        GLuint VAO = 0;
        ~Mesh(){reset();}
        void AddVertex(const Vertex& v) { vertices.push_back(v); }
        void AddIndex(const GLuint& i)  { indices.push_back(i); }
        void reset(){
            indices.clear();
            vertices.clear();
            numIndices = numVertices = 0;
            if (VBO)
                glDeleteBuffers(1, &VBO);
            if (EBO)
                glDeleteBuffers(1, &EBO);
            if (VAO)
                glDeleteVertexArrays(1, &VAO);
            material.reset();
            loaded = false;
        }
        void Load(){
            if (loaded)
                return;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            numIndices = 0;
            // Create dummy indices if necessary.
            if (!indices.size()) {
                indices.resize(vertices.size());
                for (int i = 0; i < indices.size(); ++i)
                    indices[i] = i;
            }
            numIndices = indices.size();//preserve index count for glDrawElements()
			numVertices = vertices.size();
			if (VAO)
                glDeleteVertexArrays(1, &VAO);
            if (VBO)
                glDeleteBuffers(1, &VBO);
            if (EBO)
                glDeleteBuffers(1, &EBO);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
            this->vertices.clear();
            this->indices.clear();
            glBindVertexArray(0);
            loaded = true;
        }

        void DrawElements(GLenum mode){
            glBindVertexArray(VAO);
            glDrawElements(mode, numIndices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void calculateTangents(){//Ref: https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
            for (unsigned int i = 0; i < indices.size(); i += 3){
                // Shortcuts for vertices
                int index0 = indices[i];
                int index1 = indices[i + 1];
                int index2 = indices[i + 2];

                glm::vec3& v0 = vertices[index0].position;
                glm::vec3& v1 = vertices[index1].position;
                glm::vec3& v2 = vertices[index2].position;

                // Shortcuts for UVs
                glm::vec2 uv0 = glm::vec2(vertices[index0].uv);
                glm::vec2 uv1 = glm::vec2(vertices[index1].uv);
                glm::vec2 uv2 = glm::vec2(vertices[index2].uv);

                // Edges of the triangle : postion delta
                glm::vec3 deltaPos1 = v1 - v0;
                glm::vec3 deltaPos2 = v2 - v0;

                // UV delta
                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;

                float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                // Set the same tangent for all three vertices of the triangle.
                vertices[index0].tangent = tangent;
                vertices[index1].tangent = tangent;
                vertices[index2].tangent = tangent;

                // Same thing for bitangents
                vertices[index0].bitangent = bitangent;
                vertices[index1].bitangent = bitangent;
                vertices[index2].bitangent = bitangent;
            }
            // See "Going Further"
            for (unsigned int i = 0; i < vertices.size(); i += 1)
            {
                glm::vec3& n = vertices[i].normal;
                glm::vec3& t = vertices[i].tangent;
                glm::vec3& b = vertices[i].bitangent;

                // Gram-Schmidt orthogonalize
                t = glm::normalize(t - n * glm::dot(n, t));
                b = b - n * glm::dot(b, n);
				b = b - t * glm::dot(b, t);

                // Calculate handedness
                if (glm::dot(glm::cross(n, t), b) < 0.0f) {
                    t = t * -1.0f;
                }
            }
        }
        void recalcBounds(){
            bbox.Reset();
            for (int i = 0; i < vertices.size(); ++i){
                // Find min point.
                bbox.bboxMin.x = (vertices[i].position.x < bbox.bboxMin.x)
                    ? vertices[i].position.x : bbox.bboxMin.x;
                bbox.bboxMin.y = (vertices[i].position.y < bbox.bboxMin.y)
                    ? vertices[i].position.y : bbox.bboxMin.y;
                bbox.bboxMin.z = (vertices[i].position.z < bbox.bboxMin.z)
                    ? vertices[i].position.z : bbox.bboxMin.z;

                // Find max point.
                bbox.bboxMax.x = (vertices[i].position.x > bbox.bboxMax.x)
                    ? vertices[i].position.x : bbox.bboxMax.x;
                bbox.bboxMax.y = (vertices[i].position.y > bbox.bboxMax.y)
                    ? vertices[i].position.y : bbox.bboxMax.y;
                bbox.bboxMax.z = (vertices[i].position.z > bbox.bboxMax.z)
                    ? vertices[i].position.z : bbox.bboxMax.z;
            }
			if (vertices.size() == 0)
				bbox.bboxMin = bbox.bboxMax = glm::vec3(0.0f);
        }
		friend class Scene;
        protected:
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
    };

	struct Scene 
	{
		BoundingBox bbox;
		const float minSceneBound = 100.0f;
		const float maxSceneBound = 1000.0f;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Material>> materials;
		Camera m_Camera;
		glm::vec4 m_Light = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		unsigned int triCount = 0;
		unsigned int vertexCount = 0;

		void recalcBounds()
		{
			bbox.bboxMin.xyz = glm::vec3(std::numeric_limits<float>::max());
			bbox.bboxMax.xyz = -glm::vec3(std::numeric_limits<float>::max());
			for (auto& m : this->meshes){
				// Find min point.
				bbox.bboxMin.x = (m->bbox.bboxMin.x < bbox.bboxMin.x) ? m->bbox.bboxMin.x : bbox.bboxMin.x;
				bbox.bboxMin.y = (m->bbox.bboxMin.y < bbox.bboxMin.y) ? m->bbox.bboxMin.y : bbox.bboxMin.y;
				bbox.bboxMin.z = (m->bbox.bboxMin.z < bbox.bboxMin.z) ? m->bbox.bboxMin.z : bbox.bboxMin.z;

				// Find max point.
				bbox.bboxMax.x = (m->bbox.bboxMax.x > bbox.bboxMax.x) ? m->bbox.bboxMax.x : bbox.bboxMax.x;
				bbox.bboxMax.y = (m->bbox.bboxMax.y > bbox.bboxMax.y) ? m->bbox.bboxMax.y : bbox.bboxMax.y;
				bbox.bboxMax.z = (m->bbox.bboxMax.z > bbox.bboxMax.z) ? m->bbox.bboxMax.z : bbox.bboxMax.z;
			}
		}

		void copyToOutput(std::shared_ptr<Scene> scn) 
		{
			triCount = vertexCount = 0;
			if (scn == nullptr)
				return;

			// Copy materials.
			for (int i = 0; i < scn->materials.size(); ++i)
				this->materials.push_back(scn->materials[i]);

			// Calculate dimensions for rescaling models to make them fit scene limits.
			scn->recalcBounds();
			float scaleFactor = 1.0f;
			if (scn->bbox.maxDim() < minSceneBound)
				scaleFactor = minSceneBound / scn->bbox.minDim();
			else if (scn->bbox.minDim() > maxSceneBound)
				scaleFactor = maxSceneBound / scn->bbox.maxDim();

			if (scaleFactor < 1.0e-6f)
				scaleFactor = 1.0e-6f;

			// Copy meshes, rescaling as necessary.
			if (scn->meshes.size())
			{
				for (auto x : scn->meshes)
				{
					// Add dummy material if necessary.
					if (x->material == nullptr)
						x->material = std::shared_ptr<Material>();

					// Apply scaling to vertices if necessary.
					if (scaleFactor != 1.0f)
					{
						for (auto vt : x->vertices)						
							vt.position *= scaleFactor;						
					}

					x->Load();
					triCount += x->numIndices;
					vertexCount += x->numVertices;
					this->meshes.push_back(x);
				}
			}

			triCount /= 3;
			this->recalcBounds();
		}

		~Scene(){clear();}

		void clear() {
			meshes.clear();
			materials.clear();
			bbox.Reset();
			triCount = vertexCount = 0;
		}
	};

	class Renderer 
	{
	public:
		float ambientLightBlend = 1.0f;
		float aoStrength = 1.0f;
		bool cullBackfaces = false;
		float reflectionStrength = 1.0f;
		glm::vec2 resolution = glm::vec2(0.0);
		bool useBumpMaps = false;
		bool useModelNormals = false;
		bool wireframeModeOn = false;
		std::shared_ptr<Texture> hdr_tx = nullptr;
		std::shared_ptr<Texture> hdr_irradiance_tx = nullptr;
		std::shared_ptr<Texture> hdr_prefilt_tx = nullptr;
		std::shared_ptr<Texture> lut_tx = nullptr;
		~Renderer() {
			hdr_tx->clear();
			lut_tx->clear();
			if (shader->ID)
				glDeleteProgram(shader->ID);
		}
		void init() {
			shader = defaultShader();
#ifdef _DEBUG
			checkError("After loading shaders.");
#endif
		}
		void Render();

	private:
		Shader* shader = nullptr;
		Shader* defaultShader();
	};

	struct EngineBase{
		bool isPopupHovered = false;
		bool silenceErrors = false;
		bool windowClose = false;
		std::string working_directory = "";
		std::shared_ptr<Renderer> render = nullptr;
		std::shared_ptr<Scene> scene = nullptr;
		std::shared_ptr<TextureBank> textureBank = nullptr;
		std::shared_ptr<UI> ui = nullptr;
		GLFWwindow* window = nullptr;
		
		EngineBase(GLFWwindow* wind) : window(wind) {}

		void processInput() {
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				eng->scene->m_Camera.position += eng->scene->m_Camera.front * eng->scene->m_Camera.movementSpeed;
				eng->scene->m_Camera.Update();
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				eng->scene->m_Camera.position -= eng->scene->m_Camera.front * eng->scene->m_Camera.movementSpeed;
				eng->scene->m_Camera.Update();
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				eng->scene->m_Camera.position -= eng->scene->m_Camera.right * eng->scene->m_Camera.movementSpeed;
				eng->scene->m_Camera.Update();
			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				eng->scene->m_Camera.position += eng->scene->m_Camera.right * eng->scene->m_Camera.movementSpeed;
				eng->scene->m_Camera.Update();
			}
		}

		static void resize_callback(GLFWwindow* win, int w, int h) {
			glViewport(0, 0, w, h);

			if (!eng || !eng->ui || !eng->render || !eng->scene)
				return;

			eng->ui->window_width = w;
			eng->ui->window_height = h;
			eng->ui->fileDialogSize = ImVec2(w * 0.75, h * 0.7);
			eng->render->resolution = glm::vec2(w, h);
			eng->scene->m_Camera.Update();
		}

		static void windowCloseCallback(GLFWwindow* wind)
		{
			if (eng)
				eng->windowClose = true;
		}

		void init(int w, int h) {
			std::ofstream ofs("runtime.log", std::ofstream::out | std::ofstream::trunc);
			ofs.close();
			WriteToLogFile("START 3D MODEL VIEWER LOG\r\n==========================\r\n");
			working_directory = std::filesystem::current_path().string() + '\\';
			WriteToLogFile("working directory: " + working_directory);
			eng->render = std::make_shared<Renderer>();
			eng->render->init();
			ui = std::make_shared<UI>();
			ui->desktop_width = w;
			ui->desktop_height = h;
			ui->window_width = w / 2;
			ui->window_height = h / 2;
			glfwGetWindowSize(window, &ui->window_width, &ui->window_height);
			glViewport(0, 0, ui->window_width, ui->window_height);
			ui->init();
			glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)resize_callback);
			glfwSetWindowCloseCallback(window, windowCloseCallback);
			textureBank = std::make_shared<TextureBank>();
			eng->scene = std::make_shared<Scene>();
			eng->scene->m_Camera.Update();
			WriteToLogFile("Engine Initialized.");
			windowClose = false;
		}

		void shutdown() {
			if (textureBank) {
				textureBank->clear();
				textureBank.reset();
			}
			if (render)
				render.reset();
			if (scene)
				scene.reset();
			if (ui)
				eng->ui->terminate();
			glfwTerminate();
			WriteToLogFile("3D Model Viewer Successfully Shutdown.");
		}
	};
}