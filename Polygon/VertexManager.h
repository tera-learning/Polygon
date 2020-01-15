#pragma once
#include <vector>

struct Vertex {
	int pos[2];
	float col[4];
	float tex[2];
};

class VertexManager
{
private:
	int m_VertexNum;
	std::vector<Vertex> m_VertexList;

public:
	VertexManager();
	~VertexManager();

	const Vertex* GetVertexList(void);
	int GetVertexNum(void);
	void AddVertex(const Vertex& vertex);
};