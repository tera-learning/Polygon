#include "VertexManager.h"
#include "d3d11.h"
#include <stddef.h>

VertexManager::VertexManager()
{
}

VertexManager::~VertexManager()
{
	m_VertexList.clear();
	m_VertexNum = 0;
}

const Vertex* VertexManager::GetVertexList(void)
{
	return &m_VertexList[0];
}

int VertexManager::GetVertexNum(void)
{
	return m_VertexNum;
}

void VertexManager::AddVertex(const Vertex& vertex)
{
	m_VertexList.push_back(vertex);
	m_VertexNum = m_VertexList.size();
}