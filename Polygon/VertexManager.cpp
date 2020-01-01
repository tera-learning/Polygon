#include "VertexManager.h"
#include "d3d11.h"
#include <stddef.h>

VertexManager::VertexManager()
{
	m_VertexNum = 4;

	m_VertexList.push_back({ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } });
	m_VertexList.push_back({ { -0.5f,  0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } });
	m_VertexList.push_back({ {  0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } });
	m_VertexList.push_back({ {  0.5f,  0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } });
}

VertexManager::~VertexManager()
{
	m_VertexList.clear();
	m_VertexNum = 0;
}

const Vertex* VertexManager::getVertexList(void)
{
	return &m_VertexList[0];
}

int VertexManager::getVertexNum(void)
{
	return m_VertexNum;
}