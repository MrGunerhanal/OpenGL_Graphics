#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"


class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();
	void stride();
	void aniso();

	void CreateTrack();
	void RenderTrack();

	glm::vec3 getPosition() {
		return Position;
	}
	void setPosition(glm::vec3 pos) {
		Position = pos;
	}

	glm::vec3 Position;

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up); // Return a point on the centreline based on a certain distance along the control curve.

	GLuint m_uiVAO;
	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;

	CVertexBufferObject m_vboCentreLine;
	vector<glm::vec3> m_pathPoints;
	vector<glm::vec2> m_pathUV;


	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors


	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;  // Right offset curve points	


	int m_pathVertexOffset = 1;
	unsigned int m_vertexCount;

	std::vector<glm::vec3> GetLeftOffsetCurve();
	std::vector<glm::vec3> GetRightOffsetCurve();

	std::vector<glm::vec3> GetCentrelinePoints();


private:

	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);


	vector<float> m_distances;
	CTexture m_texture;

};
