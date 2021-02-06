#include "Common.h"

#include "skybox.h"


CSkybox::CSkybox()
{}

CSkybox::~CSkybox()
{}


// Create a skybox of a given size with six textures
void CSkybox::Create(float size)
{
	/*
	m_cubemapTexture.Create(
		"resources\\skyboxes\\jajdarkland1\\\\jajdarkland1_rt.jpg", 
		"resources\\skyboxes\\jajdarkland1\\flipped\\jajdarkland1_lf.jpg",
		"resources\\skyboxes\\jajdarkland1\\flipped\\jajdarkland1_up.jpg", 
		"resources\\skyboxes\\jajdarkland1\\flipped\\jajdarkland1_dn.jpg",
		"resources\\skyboxes\\jajdarkland1\\flipped\\jajdarkland1_bk.jpg", 
		"resources\\skyboxes\\jajdarkland1\\flipped\\jajdarkland1_ft.jpg");
	*/
	
	/*
	m_cubemapTexture.Create(
		"resources\\skyboxes\\new\\flipped\\icyhell_rt.jpg",
		"resources\\skyboxes\\new\\flipped\\icyhell_lf.jpg",
		"resources\\skyboxes\\new\\flipped\\icyhell_up.jpg",
		"resources\\skyboxes\\new\\flipped\\icyhell_dn.jpg",
		"resources\\skyboxes\\new\\flipped\\icyhell_bk.jpg",
		"resources\\skyboxes\\new\\flipped\\icyhell_ft.jpg");
	*/

	m_cubemapTexture.Create(
		"resources\\skyboxes\\new\\flipped\\hangingstone_lf.jpg",
		"resources\\skyboxes\\new\\flipped\\hangingstone_rt.jpg",
		"resources\\skyboxes\\new\\flipped\\hangingstone_up.jpg",
		"resources\\skyboxes\\new\\flipped\\hangingstone_dn.jpg",
		"resources\\skyboxes\\new\\flipped\\hangingstone_bk.jpg",
		"resources\\skyboxes\\new\\flipped\\hangingstone_ft.jpg");
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();


	glm::vec3 vSkyBoxVertices[24] = 
	{
		// Front face
		glm::vec3(size, size, size), glm::vec3(size, -size, size), glm::vec3(-size, size, size), glm::vec3(-size, -size, size),
		// Back face
		glm::vec3(-size, size, -size), glm::vec3(-size, -size, -size), glm::vec3(size, size, -size), glm::vec3(size, -size, -size),
		// Left face
		glm::vec3(-size, size, size), glm::vec3(-size, -size, size), glm::vec3(-size, size, -size), glm::vec3(-size, -size, -size),
		// Right face
		glm::vec3(size, size, -size), glm::vec3(size, -size, -size), glm::vec3(size, size, size), glm::vec3(size, -size, size),
		// Top face
		glm::vec3(-size, size, -size), glm::vec3(size, size, -size), glm::vec3(-size, size, size), glm::vec3(size, size, size),
		// Bottom face
		glm::vec3(size, -size, -size), glm::vec3(-size, -size, -size), glm::vec3(size, -size, size), glm::vec3(-size, -size, size),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] = 
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	glm::vec4 vColour = glm::vec4(1, 1, 1, 1);
	for (int i = 0; i < 24; i++) {
		m_vbo.AddData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
		m_vbo.AddData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
	
}

void CSkybox::DrawQuad(glm::vec3 v0, glm::vec2 t0, glm::vec3 v1, glm::vec2 t1,
	glm::vec3 v2, glm::vec2 t2, glm::vec3 v3, glm::vec2 t3)
{
	// Set up arrays for vertices, color, and texture
	GLfloat vertexArray[12];
	GLfloat colorArray[12];
	GLfloat textureArray[8];

	vertexArray[0] = v0.x; vertexArray[1] = v0.y; vertexArray[2] = v0.z;
	vertexArray[3] = v1.x; vertexArray[4] = v1.y; vertexArray[5] = v1.z;
	vertexArray[6] = v2.x; vertexArray[7] = v2.y; vertexArray[8] = v2.z;
	vertexArray[9] = v3.x; vertexArray[10] = v3.y; vertexArray[11] = v3.z;

	colorArray[0] = 1; colorArray[1] = 1; colorArray[2] = 1;
	colorArray[3] = 1; colorArray[4] = 1; colorArray[5] = 1;
	colorArray[6] = 1; colorArray[7] = 1; colorArray[8] = 1;
	colorArray[9] = 1; colorArray[10] = 1; colorArray[11] = 1;

	textureArray[0] = t0.s; textureArray[1] = t0.t;
	textureArray[2] = t1.s; textureArray[3] = t1.t;
	textureArray[4] = t2.s; textureArray[5] = t2.t;
	textureArray[6] = t3.s; textureArray[7] = t3.t;

	// Enable the client state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Set the pointers
	glVertexPointer(3, GL_FLOAT, 0, &vertexArray[0]);
	glColorPointer(3, GL_FLOAT, 0, &colorArray[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &textureArray[0]);

	// Draw
	glDrawArrays(GL_QUADS, 0, 4);

	// Disable the client state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}


// Render the skybox
void CSkybox::Render()//float x, float y, float z, float width, float height, float length)
{
	glDepthMask(0);
	glBindVertexArray(m_vao);
	m_cubemapTexture.Bind(1);
	for (int i = 0; i < 6; i++) {
		//m_textures[i].Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	glDepthMask(1);
		
	// This centers the sky box around (x, y, z)
	//x = x - width / 2;
	//y = (y - height / 2) + 75;
	//z = z - length / 2;

	//// Specify 3D coordinates of a box around the current point
	//glm::vec3 v0 = glm::vec3(x + width, y, z);
	//glm::vec3 v1 = glm::vec3(x + width, y + height, z);
	//glm::vec3 v2 = glm::vec3(x, y + height, z);
	//glm::vec3 v3 = glm::vec3(x, y, z);
	//glm::vec3 v4 = glm::vec3(x + width, y, z + length);
	//glm::vec3 v5 = glm::vec3(x + width, y + height, z + length);
	//glm::vec3 v6 = glm::vec3(x, y + height, z + length);
	//glm::vec3 v7 = glm::vec3(x, y, z + length);

	//// Specify texture coords
	//glm::vec2 texCoord, t0, t1, t2, t3;
	//t0.s = 1.0f; t0.t = 0.0f;
	//t1.s = 1.0f; t1.t = 1.0f;
	//t2.s = 0.0f; t2.t = 1.0f;
	//t3.s = 0.0f; t3.t = 0.0f;

	//// Bind the BACK texture of the sky map to the BACK side of the cube
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_BACK_ID]);
	//DrawQuad(v0, t0, v1, t1, v2, t2, v3, t3);

	//// Bind the FRONT texture of the sky map to the FRONT side of the box
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_FRONT_ID]);
	//DrawQuad(v7, t0, v6, t1, v5, t2, v4, t3);

	//// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_BOTTOM_ID]);
	//DrawQuad(v3, t0, v7, t1, v4, t2, v0, t3);

	//// Bind the TOP texture of the sky map to the TOP side of the box
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_TOP_ID]);
	//DrawQuad(v1, t2, v5, t3, v6, t0, v2, t1);

	//// Bind the LEFT texture of the sky map to the LEFT side of the box //changed!!
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_RIGHT_ID]);
	//DrawQuad(v2, t1, v6, t2, v7, t3, v3, t0);

	//// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	//glBindTexture(GL_TEXTURE_2D, m_textureIds[SKYBOX_LEFT_ID]);
	//DrawQuad(v0, t3, v4, t0, v5, t1, v1, t2);
}

// Release the storage assocaited with the skybox
void CSkybox::Release()
{
	//for (int i = 0; i < 6; i++)
		//m_textures[i].Release();
	m_cubemapTexture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}