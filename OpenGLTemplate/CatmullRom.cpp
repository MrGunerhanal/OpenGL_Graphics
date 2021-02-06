#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>


CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	/*
	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	
	*/

	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	//m_controlPoints.push_back(glm::vec3(120, 5, 50));
	m_controlPoints.push_back(glm::vec3(200, 5, 50));
	m_controlPoints.push_back(glm::vec3(250, 5, 100));
	m_controlPoints.push_back(glm::vec3(200, 5, 150));
	m_controlPoints.push_back(glm::vec3(150, 5, 200));
	m_controlPoints.push_back(glm::vec3(100, 5, 150));
	m_controlPoints.push_back(glm::vec3(50, 5, 200));
	m_controlPoints.push_back(glm::vec3(0, 5, 150));
	m_controlPoints.push_back(glm::vec3(-200, 50, 150));
	m_controlPoints.push_back(glm::vec3(-300, 70, 400));
	m_controlPoints.push_back(glm::vec3(-450, 90, 300));
	m_controlPoints.push_back(glm::vec3(-200, 40, 200));
	m_controlPoints.push_back(glm::vec3(-100, 20, 100));
	m_controlPoints.push_back(glm::vec3(-50, 20, -100));
	
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p;
	glm::vec3 up(0.0f, 0.0f, 0.0f);

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}
}



void CCatmullRom::CreateCentreline()
{

	// Call Set Control Points
	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card	

	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	CVertexBufferObject m_vboCentreline;

	m_vboCentreline.Create();
	m_vboCentreline.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < m_controlPoints.size() - 3; i++) {
		float t = (float)i / 100.0f;
		glm::vec3 v = Interpolate(m_controlPoints[i], m_controlPoints[i + 1], m_controlPoints[i + 2], m_controlPoints[i + 3], t);
		m_vboCentreline.AddData(&v, sizeof(glm::vec3));
		m_vboCentreline.AddData(&texCoord, sizeof(glm::vec2));
		m_vboCentreline.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	m_vboCentreline.UploadDataToGPU(GL_STATIC_DRAW);
	stride();

}



void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right. Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	for (unsigned int i = 0; i < m_centrelinePoints.size() - 3; i++) {

		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext;

		if (i + 1 < m_centrelinePoints.size()) {
			pNext = m_centrelinePoints[i + 1];
		}
		else {
			pNext = m_centrelinePoints[0];
		}

		glm::vec3 tangent = glm::normalize(pNext - p);
		glm::vec3 y = glm::vec3(0, 1, 0);
		glm::vec3 normal = glm::normalize(glm::cross(tangent, y));
		glm::vec3 binormal = glm::normalize(glm::cross(normal, tangent));

		glm::vec3 left = p - 15.0f * normal;
		m_leftOffsetPoints.push_back(left);

		glm::vec3 right = p + 15.0f * normal;
		m_rightOffsetPoints.push_back(right);
	}
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	CVertexBufferObject m_vboLeftOffsetCurve;
	m_vboLeftOffsetCurve.Create();
	m_vboLeftOffsetCurve.Bind();

	glm::vec2 ltexCoord(0.0f, 0.0f);
	glm::vec3 lnormal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_leftOffsetPoints.size() - 3; i++) {
		float t = (float)i / 100.0f;
		glm::vec3 l = Interpolate(m_leftOffsetPoints[i], m_leftOffsetPoints[i + 1], m_leftOffsetPoints[i + 2], m_leftOffsetPoints[i + 3], t);

		m_vboLeftOffsetCurve.AddData(&l, sizeof(glm::vec3));
		m_vboLeftOffsetCurve.AddData(&ltexCoord, sizeof(glm::vec2));
		m_vboLeftOffsetCurve.AddData(&lnormal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	//m_vboLeftOffsetCurve.UploadDataToGPU(GL_STATIC_DRAW);
	//stride();

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	CVertexBufferObject m_vboRightOffsetCurve;

	m_vboRightOffsetCurve.Create();
	m_vboRightOffsetCurve.Bind();

	glm::vec2 rtexCoord(0.0f, 0.0f);
	glm::vec3 rnormal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_rightOffsetPoints.size() - 3; i++) {
		float t = (float)i / 100.0f;
		glm::vec3 r = Interpolate(m_rightOffsetPoints[i], m_rightOffsetPoints[i + 1], m_rightOffsetPoints[i + 2], m_rightOffsetPoints[i + 3], t);

		m_vboRightOffsetCurve.AddData(&r, sizeof(glm::vec3));
		m_vboRightOffsetCurve.AddData(&rtexCoord, sizeof(glm::vec2));
		m_vboRightOffsetCurve.AddData(&rnormal, sizeof(glm::vec3));
	}

	//m_vboRightOffsetCurve.UploadDataToGPU(GL_STATIC_DRAW);
	//stride();

}

std::vector<glm::vec3> CCatmullRom::GetCentrelinePoints() {

	return m_centrelinePoints;
}


std::vector<glm::vec3> CCatmullRom::GetLeftOffsetCurve() {

	return m_leftOffsetPoints; 
}

std::vector<glm::vec3> CCatmullRom::GetRightOffsetCurve() {

	return m_rightOffsetPoints;
}


void CCatmullRom::CreateTrack()
{
	//texture image loaded
	m_texture.Load("resources\\textures\\road4.png", true);

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject m_vboTrack;
	m_vboTrack.Create();
	m_vboTrack.Bind();

	glm::vec2 texCoord00(0.0f, 0.0f);
	glm::vec2 texCoord01(0.0f, 1);
	glm::vec2 texCoord10(0.1f, 0);
	glm::vec2 texCoord11(0.1f, 1);

		m_vertexCount = (int)m_rightOffsetPoints.size() - m_pathVertexOffset;

		for (int i = 0; i < m_vertexCount; i += m_pathVertexOffset)
		{
			m_pathPoints.push_back(m_leftOffsetPoints[i]);
			m_pathUV.push_back(texCoord00);

			m_pathPoints.push_back(m_rightOffsetPoints[i]);
			m_pathUV.push_back(texCoord01);

			m_pathPoints.push_back(m_leftOffsetPoints[i + m_pathVertexOffset]);
			m_pathUV.push_back(texCoord10);

			m_pathPoints.push_back(m_leftOffsetPoints[i + m_pathVertexOffset]);
			m_pathUV.push_back(texCoord10);

			m_pathPoints.push_back(m_rightOffsetPoints[i]);
			m_pathUV.push_back(texCoord01);

			m_pathPoints.push_back(m_rightOffsetPoints[i + m_pathVertexOffset]);
			m_pathUV.push_back(texCoord11);

		}

		m_pathPoints.push_back(m_leftOffsetPoints[m_vertexCount]);
		m_pathUV.push_back(texCoord00);

		m_pathPoints.push_back(m_rightOffsetPoints[m_vertexCount]);
		m_pathUV.push_back(texCoord01);

		m_pathPoints.push_back(m_leftOffsetPoints[0]);
		m_pathUV.push_back(texCoord10);

		m_pathPoints.push_back(m_leftOffsetPoints[0]);
		m_pathUV.push_back(texCoord10);

		m_pathPoints.push_back(m_rightOffsetPoints[0]);
		m_pathUV.push_back(texCoord01);

		m_pathPoints.push_back(m_rightOffsetPoints[m_vertexCount]);
		m_pathUV.push_back(texCoord11);

	m_vertexCount = (int)m_pathPoints.size();

	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	
	for (unsigned int i = 0; i < m_vertexCount; i++)
	{
		m_vboTrack.AddData(&m_pathPoints[i], sizeof(glm::vec3));
		m_vboTrack.AddData(&m_pathUV[i], sizeof(glm::vec2));
		m_vboTrack.AddData(&normal, sizeof(glm::vec3));
	}
	
	// Upload the VBO to the GPU
	m_vboTrack.UploadDataToGPU(GL_STATIC_DRAW);
	stride();
}

void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size() - 3);
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size() - 3);

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size() - 3);
}

void CCatmullRom::RenderTrack()
{
	
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);	
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	aniso();
}

int CCatmullRom::CurrentLap(float d)
{
	return (int)(d / m_distances.back());
}

void::CCatmullRom::aniso() {
	
	GLfloat aniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
}

void CCatmullRom::stride()
{
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}