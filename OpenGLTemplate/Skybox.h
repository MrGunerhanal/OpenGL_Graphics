#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"
#include "Cubemap.h"

typedef enum {
	SKYBOX_BACK_ID,
	SKYBOX_FRONT_ID,
	SKYBOX_BOTTOM_ID,
	SKYBOX_TOP_ID,
	SKYBOX_LEFT_ID,
	SKYBOX_RIGHT_ID
} BoxSideTypes;

// This is a class for creating and rendering a skybox
class CSkybox
{
public:
	CSkybox();
	~CSkybox();
	void Create(float size);
	void Render();//(float x, float y, float z, float width, float height, float length);
	void DrawQuad(glm::vec3 v0, glm::vec2 t0, glm::vec3 v1, glm::vec2 t1,
		glm::vec3 v2, glm::vec2 t2, glm::vec3 v3, glm::vec2 t3);
	void Release();

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CCubemap m_cubemapTexture;
	unsigned int m_textureIds[6];
	
};