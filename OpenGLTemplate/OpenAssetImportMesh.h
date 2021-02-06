/*

	Copyright 2011 Etay Meiri

	// Modified by Dr Greg Slabaugh to work with OpenGL template

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <map>
#include <vector>
#include "include/gl/glew.h"
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <PostProcess.h> // Post processing flags

#include "Common.h"
#include "Texture.h"

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }


struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;
    glm::vec3 m_normal;

    Vertex() {}

    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }
};


class COpenAssetImportMesh
{
public:
    COpenAssetImportMesh();
    ~COpenAssetImportMesh();
    bool Load(const std::string& Filename);
    void Render();

    glm::vec3 GetPosition() const;

	void setPosition(glm::vec3 pos) {
		Position = pos;
	}
	glm::mat4 getRotation() {
		return m_orientation;
	}
	void setRotation(glm::mat4 rot) {
		m_orientation = rot;
	}

    // Respond to keyboard presses on arrow keys to translate the camera
    void TranslateByKeyboard(double dt);

    // Strafe the camera (move it side to side)
    void Strafe(double direction);

    // Advance the camera (move it forward or backward)
    void Advance(double direction);

    // Update the camera
    void Update(double dt);


private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

	glm::vec3 Position;
	glm::mat4 m_orientation;

    glm::vec3 m_position;			// The position of the camera's centre of projection
    glm::vec3 m_view;				// The camera's viewpoint (point where the camera is looking)
    glm::vec3 m_upVector;			// The camera's up vector
    glm::vec3 m_strafeVector;		// The camera's strafe vector
	

    float m_speed;
#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
                  const std::vector<unsigned int>& Indices);
        GLuint vbo;
        GLuint ibo;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<CTexture*> m_Textures;
	GLuint m_vao;
};



