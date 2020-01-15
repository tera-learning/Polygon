#pragma once
#include <vector>

struct Vertex {
	int pos[2];
	float col[4];
	float tex[2];
};

class VertexManager final
{
private:
	int m_VertexNum;
	std::vector<Vertex> m_VertexList;

public:
	VertexManager();
	VertexManager(const VertexManager& vertexManager) = delete;
	VertexManager& operator=(const VertexManager& vertexManager) = delete;
	/*virtual*/ ~VertexManager();

	const Vertex* GetVertexList(void);
	int GetVertexNum(void);
	void AddVertex(const Vertex& vertex);
};