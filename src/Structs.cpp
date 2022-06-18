#include "structs.hpp"

namespace TDModelView 
{
	void TextureBank::remove(std::string filename) 
	{
		for (int i = 0; i < eng->scene->materials.size(); ++i) {
			for (int j = 0; j < eng->scene->materials[i]->textures.size(); ++j)
			{
				if (eng->scene->materials[i]->textures[j] != nullptr
					&& eng->scene->materials[i]->textures[j]->filepath == filename)
				{
					eng->scene->materials[i]->textures[j] = nullptr;
				}
			}
		}
		for (int i = 0; i < textures.size(); ++i) {
			if (textures[i].filepath == filename)
			{
				textures[i].clear();
				textures.erase(textures.begin() + i);
			}
		}
	}

	void Camera::Update()
	{
		if (up == glm::vec3(0.0f))
			up = glm::vec3(0.0f, 0.0f, 1.0f);		
		up = glm::normalize(glm::vec3(WorldUp) * rotation);
		right = normalize(glm::vec3(WorldRight) * rotation);
		front = normalize(glm::vec3(WorldFront) * rotation);
		aspect = (float)eng->ui->window_width / (float)eng->ui->window_height;
		angle = 1.0f / (aspect * tan(0.5f * fov));
		glm::mat4 P = glm::perspective(fov_rad, aspect, zNear, zFar);
		glm::mat4 V = glm::lookAt(position, position + front, up);
		VP = P * V;
	}

	void Renderer::Render() 
	{
		if (eng->windowClose || eng->scene->meshes.size() == 0 || eng->ui->showFileDialog)
			return;
		shader->use();

		// static uniform locs.
		static GLuint UcameraPosition = glGetUniformLocation(shader->ID, "cameraPosition");
		static GLuint Ulvec = glGetUniformLocation(shader->ID, "lightVec");
		static GLuint Umodmat = glGetUniformLocation(shader->ID, "modelMatrix");
		static GLuint Umvp = glGetUniformLocation(shader->ID, "modelViewProjection");
		static GLuint Unmat = glGetUniformLocation(shader->ID, "normalMatrix");
		static GLuint UuseBmaps = glGetUniformLocation(shader->ID, "useBumpMap");
		static GLuint UambLightBlend = glGetUniformLocation(shader->ID, "ambientLightBlend");
		static GLuint UaoStrength = glGetUniformLocation(shader->ID, "aoStrength");
		static GLuint UreflStr = glGetUniformLocation(shader->ID, "reflectionStrength");
		static GLuint Uresolution = glGetUniformLocation(shader->ID, "resolution");


		// Set uniforms:
		glUniform1i(UuseBmaps, useBumpMaps);
		glUniform3fv(UcameraPosition, 1, &eng->scene->m_Camera.position[0]);
		glUniform4fv(Ulvec, 1, &eng->scene->m_Light[0]);
		glUniform1f(UambLightBlend, ambientLightBlend);
		glUniform1f(UaoStrength, aoStrength);
		glUniform1f(UreflStr, reflectionStrength);
		glUniform2fv(Uresolution, 1, &resolution[0]);


		for (auto m : eng->scene->meshes) {
			glUniformMatrix4fv(Umodmat, 1, GL_FALSE, &m->modelMatrix[0][0]);
			glm::mat4 MVP = eng->scene->m_Camera.VP * m->modelMatrix;
			glUniformMatrix4fv(Umvp, 1, GL_FALSE, &MVP[0][0]);
			glm::mat3 nMat = glm::transpose(glm::inverse(glm::mat3(m->modelMatrix)));
			glUniformMatrix3fv(Unmat, 1, GL_FALSE, &nMat[0][0]);
			m->material->setUniforms(shader, useModelNormals);

			// Bind textures:
			for (int i = 0; i < aiTextureType_UNKNOWN; ++i){
				glActiveTexture(GL_TEXTURE0 + i);
				if (m->material->HasTexture(aiTextureType(i))) {
					m->material->BindTexture(aiTextureType(i));
				}
				else if (i == (int)aiTextureType_REFLECTION){
					glBindTexture(GL_TEXTURE_2D, hdr_tx->id);
				}
			}
			glActiveTexture(GL_TEXTURE18);// bind brdf pre-calc'd lut
			glBindTexture(GL_TEXTURE_2D, lut_tx->id);
			
			glActiveTexture(GL_TEXTURE19);
			glBindTexture(GL_TEXTURE_2D, hdr_irradiance_tx->id);

			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, hdr_prefilt_tx->id);


			
			m->DrawElements(eng->render->wireframeModeOn ? GL_LINES : GL_TRIANGLES);
#ifdef _DEBUG
			checkError("After rendering model");
#endif
		}
	}
}