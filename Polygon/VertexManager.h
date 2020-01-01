#pragma once
#include <vector>

struct Vertex {
	float pos[3];
	float col[4];
};

class VertexManager
{
private:
	int m_VertexNum;
	std::vector<Vertex> m_VertexList;

public:
	VertexManager();
	~VertexManager();

	const Vertex* getVertexList(void);
	int getVertexNum(void);
};