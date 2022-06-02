#include "structs.hpp"

namespace TDModelView 
{
	Shader* Renderer::defaultShader() 
	{
		Shader* shader = new Shader();
		const char* vert =
			"#version 330\n"
			"layout(location = 0) in vec3 vertexPosition;\n"
			"layout(location = 1) in vec3 vertexTexCoord;\n"
			"layout(location = 2) in vec3 vertexNormal;\n"
			"layout(location = 3) in vec3 vertexTangent;\n"
			"layout(location = 4) in vec3 vertexBitangent;\n"
			"uniform mat3 normalMatrix;\n"
			"uniform mat4 modelMatrix;\n"
			"uniform mat4 modelViewProjection;\n"
			"out Vertex{\n"
			"	vec3 position;\n"
			"	vec3 texCoord;\n"
			"	vec3 normal;\n"
			"	vec3 tangent;\n"
			"	vec3 bitangent;\n"
			"	vec3 TangentViewPos;\n"
			"	vec3 TangentFragPos;\n"
			"};\n"
			"uniform struct Material {\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n"
			"	vec3 ambient;\n"
			"	vec3 emissive;\n"
			"	vec3 transparent;\n"
			"	float specularFactor;\n"
			"	float roughness;\n"
			"	float opacity;\n"
			"	float ior;\n"
			"	float metalness;\n"
			"} material;\n"
			"layout(binding = 5) uniform sampler2D heightMap;\n"
			"uniform bool hasHeightMap = false;\n"
			"uniform vec3 heightMapTranslation = vec3(0);\n"
			"uniform vec3 heightMapScale = vec3(1);\n"
			"uniform float heightMapAmplitude = 1.0;\n"
			"uniform int heightMapUseChannel = 0;\n"
			"layout(binding = 9) uniform sampler2D displacementMap;\n"
			"uniform bool hasDisplacementMap = false;\n"
			"uniform vec3 displacementMapTranslation = vec3(0);\n"
			"uniform vec3 displacementMapScale = vec3(1);\n"
			"uniform float displacementMapAmplitude = 1.f;\n"
			"uniform int displacementMapUseChannel = 0;\n"
			"uniform vec3 cameraPosition;\n"
			"vec3 TF(mat4 M, vec3 v){\n"
			"	vec4 v2 = M * vec4(v,1.0);\n"
			"	return v2.xyz / v2.w;"
			"}\n"
			"void main() {\n"
			"	vec4 vertexPos = vec4(vertexPosition, 1.0);\n"
			"	if (hasHeightMap) {\n"
			"		float heightVal = texture(heightMap, vertexTexCoord.xy).r;\n"
			"		heightVal = heightVal * 2.0 - 1.0;\n"//scale [0,1] ==> [-1,1]
			"		heightVal *= heightMapAmplitude;\n"
			"		vertexPos.xyz += heightVal * vertexNormal;\n"
			"	}\n"
			"	position = TF(modelMatrix, vertexPos.xyz);\n"
			"	texCoord = vertexTexCoord;\n"
			"	normal = normalize(normalMatrix * vertexNormal);\n"
			"	tangent = normalize(normalMatrix * vertexTangent);\n"
			"	bitangent = normalize(normalMatrix * vertexBitangent);\n"
			"	if (hasDisplacementMap) {\n"
			"		mat3 TBN = mat3(tangent, bitangent, normal);\n"
			"		TangentViewPos = TBN * cameraPosition;\n"
			"		TangentFragPos = TBN * position;\n"
			"	}\n"
			"	gl_Position = modelViewProjection * vertexPos;\n"
			"}\n";

		const char* frag =
			"#version 330\n"
			"out vec4 fragColor;\n"
			"in Vertex{\n"
			"	vec3 position;\n"
			"	vec3 texCoord;\n"
			"	vec3 normal;\n"
			"	vec3 tangent;\n"
			"	vec3 bitangent;\n"
			"	vec3 TangentViewPos;\n"
			"	vec3 TangentFragPos;\n"
			"};\n"
			"uniform mat3 normalMatrix;\n"
			"uniform mat4 modelMatrix;\n"
			"uniform mat4 modelViewProjection;\n"
			"uniform struct Material {\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n"
			"	vec3 ambient;\n"
			"	vec3 emissive;\n"
			"	vec3 transparent;\n"
			"	float specularFactor;\n"
			"	float roughness;\n"
			"	float opacity;\n"
			"	float ior;\n"
			"	float metalness;\n"
			"} material;\n"
			"layout(binding = 1) uniform sampler2D diffuseMap;\n"
			"uniform bool hasDiffuseMap = false;\n"
			"layout(binding = 2) uniform sampler2D specularMap;\n"
			"uniform bool hasSpecularMap = false;\n"
			"layout(binding = 3) uniform sampler2D ambientMap;\n"
			"uniform bool hasAmbientMap = false;\n"
			"layout(binding = 4) uniform sampler2D emissiveMap;\n"
			"uniform bool hasEmissiveMap = false;\n"
			"layout(binding = 6) uniform sampler2D normalsMap;\n"
			"uniform bool hasNormalMap = false;\n"
			"uniform bool useBumpMap = false;\n"
			"layout(binding = 7) uniform sampler2D shininessMap;\n"
			"uniform bool hasShininessMap = false;\n"
			"layout(binding = 8) uniform sampler2D opacityMap;\n"
			"uniform bool hasOpacityMap = false;\n"
			"layout(binding = 9) uniform sampler2D displacementMap;\n"
			"uniform bool hasDisplacementMap = false;\n"
			"uniform float displacementMapAmplitude = 1.f;\n"
			"uniform float displacementMapBias = 0.f;\n"
			"layout(binding = 11) uniform sampler2D reflectionMap;\n"
			"uniform bool hasReflectionMap = false;\n"
			"layout(binding = 12) uniform sampler2D albedoMap;\n"
			"uniform bool hasAlbedoMap = false;\n"
			"layout(binding = 14) uniform sampler2D emissivecolorMap;\n"
			"uniform bool hasEmissiveColorMap = false;\n"
			"layout(binding = 15) uniform sampler2D metalnessMap;\n"
			"uniform bool hasMetalnessMap = false;\n"
			"layout(binding = 16) uniform sampler2D roughnessMap;\n"
			"uniform bool hasRoughnessMap = false;\n"
			"layout(binding = 17) uniform sampler2D ambientocclusionMap;\n"
			"uniform bool hasAmbientOcclusionMap = false;\n"
			"uniform float alphaCutoff = 0.01f;\n"
			"uniform int parallaxSamples = 10;\n"
			"uniform vec3 cameraPosition;\n"
			"uniform vec4 lightVec;\n"
			"float D_GGX(float NdotH, float roughPow2) {\n"
			"	float oneMinusNoHSquared = 1.0 - NdotH * NdotH;\n"
			"	float a = NdotH * roughPow2;\n"
			"	float k = roughPow2 / (oneMinusNoHSquared + a * a);\n"
			"	return clamp(k * k * 0.3183098861838f, 0.0f, 1.0f);\n"
			"}\n"
			"float V_GGX(float NdotL, float NdotV, float alphaRoughness) {\n"
			"	float alphaRoughnessSq = alphaRoughness * alphaRoughness;\n"
			"	float GGXV = NdotL * sqrt(max(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq, 0.0f));\n"
			"	float GGXL = NdotV * sqrt(max(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq, 0.0f));\n"
			"	float GGX = GGXV + GGXL;\n"
			"	return max(0.5 / GGX, 0.0f);\n"
			"}\n"
			"vec3 F_Schlick(vec3 f0, vec3 f90, float VdotH) {\n"
			"	return max(f0 + clamp(f90 - f0, 0.0f, 1.0f) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f), 0.0f);\n"
			"}\n"
			"vec3 BRDF_specularGGX(vec3 f0, vec3 f90, float alphaRoughness, float VdotH, float NdotL, float NdotV, float NdotH) {\n"
			"	return F_Schlick(f0, f90, VdotH) * V_GGX(NdotL, NdotV, alphaRoughness) * D_GGX(NdotH, alphaRoughness);\n"
			"}\n"
			"vec3 ACES(vec3 x){return (x*(2.51f*x+0.03f))/(x*(2.43f*x+0.59f)+0.14f);}\n"
			"vec3 bumpMapping(vec3 texCoord2) {\n"
			"	vec3 norm = normalize(normal);\n"
			"	vec3 tang = normalize(tangent);\n"
			"	tang = normalize(tang - dot(tang, norm) * norm);\n"
			"	vec3 bTan = normalize(bitangent);\n"
			"	mat3 TBN = mat3(tang, bTan, norm);\n"
			"	float bumpVal = texture(normalsMap, texCoord2.xy).r;\n"
			"	vec3 tNormal = vec3(dFdx(bumpVal), dFdy(bumpVal), 0.0);\n"
			"	tNormal.z = 1.0 - tNormal.x - tNormal.y;\n"
			"	tNormal = 2.0 * tNormal - 1.0;\n"
			"	tNormal = normalize(TBN * tNormal);\n"
			"	tNormal = length(tNormal) > 0.001f ? tNormal : norm;\n"//sanity check to prevent null vector.
			"	return tNormal;\n"
			"}\n"
			"vec3 normalMapping(vec3 texCoord2) {\n"
			"	vec3 norm = normalize(normal);\n"
			"	vec3 tang = normalize(tangent);\n"
			"	tang = normalize(tang - dot(tang, norm) * norm);\n"
			"	vec3 bTan = normalize(bitangent);\n"
			"	mat3 TBN = mat3(tang, bTan, norm);\n"
			"	vec3 tNormal = texture(normalsMap, texCoord2.xy).rgb;\n"
			"	tNormal = 2.0 * tNormal - 1.0;\n"
			"	tNormal = normalize(TBN * tNormal);\n"
			"	tNormal = length(tNormal) > 0.001f ? tNormal : norm;\n"//sanity check to prevent null vector.
			"	return tNormal;\n"
			"}\n"
			"vec2 parallax() {\n"
			"	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);\n"
			"	const float minLayers = 8;\n"
			"	float numLayers = mix(parallaxSamples, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));\n"
			"	float layerDepth = 1.0 / numLayers;\n"
			"	float currentLayerDepth = 0.0;\n"
			"	vec2 P = (viewDir.xy - displacementMapBias) / viewDir.z * displacementMapAmplitude;\n"
			"	vec2 deltaTexCoords = P / numLayers;\n"
			"	vec2  currentTexCoords = texCoord.xy;\n"
			"	float currentDepthMapValue = texture(displacementMap, texCoord.xy).r;\n"
			"	while (currentLayerDepth < currentDepthMapValue) {\n"
			"		currentTexCoords -= deltaTexCoords;\n"
			"		currentDepthMapValue = texture(displacementMap, currentTexCoords.xy).r;\n"
			"		currentLayerDepth += layerDepth;\n"
			"	}\n"
			"	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;\n"
			"	float afterDepth = currentDepthMapValue - currentLayerDepth;\n"
			"	float beforeDepth = texture(displacementMap, prevTexCoords.xy).r - currentLayerDepth + layerDepth;\n"
			"	float weight = afterDepth / (afterDepth - beforeDepth);\n"
			"	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);\n"
			"	return finalTexCoords;\n"
			"}\n"
			"vec2 equirect2uv(vec3 worldPos) {\n"
			"	float n = length(worldPos.xz);\n"
			"	vec2 pos = vec2((n > 0.0000001) ? worldPos.x / n : 0.0, worldPos.y);\n"
			"	pos = acos(pos) * 0.31831;\n"
			"	pos.x = (worldPos.z > 0.0) ? pos.x * 0.5 : 1.0 - (pos.x * 0.5);\n"
			"	pos.x = 1.0 - pos.x;\n"
			"	return pos;\n"
			"}\n"
			"vec2 equipolar2uv(vec3 worldPos) {\n"
			"	worldPos.xy /= abs(worldPos.z) + 1.0;\n"
			"	worldPos.xy = worldPos.xy * 0.5 + 0.5;\n"
			"	worldPos.x *= 0.5;\n"
			"	worldPos.x += sign(-worldPos.z) * 0.25 + 0.25;\n"
			"	return worldPos.xy;\n"
			"}\n"
			"void main() {\n"
			"	vec3 newTexCoord = texCoord;\n"
			"	if (hasDisplacementMap) {\n" // Apply parallax effect.
			"		newTexCoord.xy = parallax();\n"
			"	}\n"
			"	vec4 diffuseColor = vec4(material.diffuse, 1.0);\n"
			"	if (hasAlbedoMap) {\n"
			"		vec4 dif = texture(albedoMap, newTexCoord.xy);\n"
			"		diffuseColor *= dif;\n"
			"	}\n"
			"	else if (hasDiffuseMap) {\n"
			"		vec4 dif = texture(diffuseMap, newTexCoord.xy);\n"
			"		diffuseColor *= dif;\n"
			"	}\n"
			"	float opacityVal = min(material.opacity, diffuseColor.a);\n"
			"	if (hasOpacityMap) {\n"
			"		opacityVal = texture(opacityMap, newTexCoord.xy).r;\n"
			"	}\n"
			"	if (opacityVal <= alphaCutoff) {\n"
			"		discard;\n"
			"		return;\n"
			"	}\n"
			"	vec4 gAlbedo = vec4(diffuseColor.rgb, opacityVal);\n"
			"	if(gAlbedo.a <= 0.0f){ discard; return; }\n" // Do transparency fragment discard here
			"	vec4 specularColor = vec4(material.specular, material.specularFactor);\n"
			"	if (hasSpecularMap) {\n"
			"		vec4 spec = texture(specularMap, newTexCoord.xy);\n"
			"		specularColor.rgb *= spec.rgb;\n"
			"	}\n"
			"	vec4 gSpecular = vec4(specularColor.rgb, specularColor.a);\n"
			"	vec3 nml = normal;\n"
			"	if (useBumpMap)\n"
			"		nml = bumpMapping(newTexCoord).xyz;\n"
			"	else if (hasNormalMap)\n"
			"		nml = normalMapping(newTexCoord).xyz;\n"
			"	vec3 emissiveColor = material.emissive;\n"
			"	if (hasEmissiveColorMap) {\n"
			"		emissiveColor = texture(emissivecolorMap,newTexCoord.xy).rgb;\n"
			"		if (hasEmissiveMap){\n"
			"			vec3 emis = texture(emissiveMap,newTexCoord.xy).rgb;\n"
			"			emissiveColor *= emis;\n"
			"		}\n"
			"	}\n"
			"	else if (hasEmissiveMap)\n"
			"		emissiveColor = texture(emissiveMap,newTexCoord.xy).rgb;\n"
			"	float ao = 1.0;\n"
			"	if (hasAmbientOcclusionMap) {\n"
			"		ao = clamp(pow(texture(ambientocclusionMap, newTexCoord.xy).r, 0.2f), 0.0f, 1.0f);\n"
			"	}\n"
			"	float rough = material.roughness;\n"
			"	if (hasShininessMap){\n"
			"		rough = max(1.0f - texture(shininessMap, newTexCoord.xy).r, 0.0f);\n"
			"	}\n"
			"	else if (hasRoughnessMap) {\n"
			"		rough = texture(roughnessMap, newTexCoord.xy).r;\n"
			"	}\n"
			"	float metalness = material.metalness;\n"
			"	if (hasMetalnessMap){\n"
			"		metalness = max(texture(metalnessMap, newTexCoord.xy).r, 0.0f);\n"
			"	}\n"
			"	float ior = clamp(material.ior, 1.0f, 50.0f);\n"
			"	rough = clamp(rough, 0.001f, 0.995f);\n"
			"	metalness = clamp(metalness, 0.001f, 0.995f);\n"
			"	vec4 reflectionColor = texture(reflectionMap, equirect2uv(reflect(normalize(position - cameraPosition), nml)));\n"// always use reflection map

			// Lighting section
			"	vec3 N = nml;\n"
			"	vec3 L = normalize(lightVec.rgb);\n"
			"	vec3 L2 = normalize(-lightVec.rgb);\n"
			"	vec3 V = normalize(cameraPosition - position);\n"
			"	vec3 H = normalize(L + V);\n"
			"	float NdotL = max(dot(N, L), 0.0f);\n"
			"	float NdotL2 = max(dot(N, L2), 0.0f);\n"
			"	float NdotV = max(dot(N, V), 0.0f);\n"
			"	float NdotH = max(dot(N, H), 0.0f);\n"
			"	float LdotV = max(dot(L, V), 0.0f);\n"
			"	float LdotH = max(dot(L, H), 0.0f);\n"
			"	float VdotH = max(dot(V, H), 0.0f);\n"

			// Calculate Fresnel reflectance at 0 and 90 angles. Metal-rough workflow, use ior for reflectance.
			"	vec3 F0 = clamp(vec3(pow((ior-1.0f)/(ior+1.0f), 2.0f)), vec3(0.01f), vec3(10.0f));\n"
			"	vec3 albedoColor = mix(gAlbedo.rgb * (1.0f - F0), vec3(0.0f), metalness);\n"
			"	F0 = mix(F0, albedoColor, metalness);\n"			
			"	float reflectance = max(max(F0.r, F0.g), F0.b);\n"
			"	vec3 F90 = vec3(clamp(reflectance * metalness * 50.0f, 0.01f, 10.0f));\n"
			
			"	vec3 diffuse_ = albedoColor * (NdotL + NdotL2);\n"
			"	vec3 specularStrength = (NdotL + NdotL2) * (BRDF_specularGGX(F0, F90, rough*rough, VdotH, NdotL, NdotV, NdotH) + BRDF_specularGGX(F0, F90, rough*rough, VdotH, NdotL2, NdotV, NdotH));\n"
			"	vec3 specular_ = specularStrength * (gSpecular.rgb + (reflectionColor.rgb ));"//* (1.0f - rough) * ao));\n"
			"	vec3 result = lightVec.a*(diffuse_ + specular_) + emissiveColor;\n" // also add constant ambient term
			"	fragColor = vec4(clamp(ACES(result), vec3(0.0f), vec3(1.0f)), 1.0f);\n"
			"}\n";

		// Compile vertex shader.
		char infoLog[1024];
		unsigned int vert_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_id, 1, &vert, NULL);
		glCompileShader(vert_id);
		int success;
		glGetShaderiv(vert_id, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vert_id, 1024, NULL, infoLog);
			std::string errorStr = std::string(infoLog);
			ErrorMessageBox(errorStr);
		}

		// Compile frag shader.
		unsigned int frag_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_id, 1, &frag, NULL);
		glCompileShader(frag_id);
		glGetShaderiv(frag_id, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(frag_id, 1024, NULL, infoLog);
			std::string errorStr = std::string(infoLog);
			ErrorMessageBox(errorStr);
		}

		// Attach and compile all.
		shader->handle = "defaultShader";
		shader->ID = glCreateProgram();
		glAttachShader(shader->ID, vert_id);
		glAttachShader(shader->ID, frag_id);
		glLinkProgram(shader->ID);
		shader->checkCompileErrors(shader->ID, "defaultShader");

		// Cleanup.
		glDeleteShader(vert_id);
		glDeleteShader(frag_id);
		return shader;
	}
}