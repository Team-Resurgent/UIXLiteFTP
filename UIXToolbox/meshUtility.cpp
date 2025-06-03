#include "meshUtility.h"

utils::dataContainer* meshUtility::createQuadXY(const math::vec3F& position, const math::sizeF& size, const math::rectF& uvRect)
{
    vertex v1 = vertex(position.x, position.y, position.z, uvRect.x, uvRect.y + uvRect.height);
    vertex v2 = vertex(position.x + size.width, position.y, position.z, uvRect.x + uvRect.width, uvRect.y + uvRect.height);
    vertex v3 = vertex(position.x + size.width, position.y + size.height, position.z, uvRect.x + uvRect.width, uvRect.y);
    vertex v4 = vertex(position.x, position.y + size.height, position.z, uvRect.x, uvRect.y);

	utils::dataContainer* vertices = new utils::dataContainer();
	vertices->size = 6 * sizeof(vertex);
	vertices->data = (char*)malloc(vertices->size);
	
	memcpy(vertices->data + (0 * sizeof(vertex)), &v3, sizeof(vertex));
	memcpy(vertices->data + (1 * sizeof(vertex)), &v2, sizeof(vertex));
	memcpy(vertices->data + (2 * sizeof(vertex)), &v1, sizeof(vertex));
	memcpy(vertices->data + (3 * sizeof(vertex)), &v3, sizeof(vertex));
	memcpy(vertices->data + (4 * sizeof(vertex)), &v1, sizeof(vertex));
	memcpy(vertices->data + (5 * sizeof(vertex)), &v4, sizeof(vertex));

    return vertices;
}

utils::dataContainer* meshUtility::createNinePatchXY(const math::vec3F& position, const math::sizeF& size, const math::rectF& uvRect)
{
	utils::dataContainer* tempVertices = new utils::dataContainer();
	tempVertices->size = (4 * 4) * sizeof(vertex);
	tempVertices->data = (char*)malloc(tempVertices->size);

	uint32_t tempVertexIndex = 0;
    for (int y = 0; y < 4; y++)
    {
		float piecePosY = math::calcNinePatchPosition(y, size.height) + position.y;
		float pieceUVY = math::calcNinePatchUV(y, uvRect.height) + uvRect.y;
        for (int x = 0; x < 4; x++)
        {
			float piecePosX = math::calcNinePatchPosition(x, size.width) + position.x;
			float pieceUVX = math::calcNinePatchUV(x, uvRect.width) + uvRect.x;
			vertex v = vertex(piecePosX, piecePosY, 0, pieceUVX, pieceUVY);
			memcpy(tempVertices->data + (tempVertexIndex * sizeof(vertex)), &v, sizeof(vertex));
			tempVertexIndex++;
        }
    }

	utils::dataContainer* vertices = new utils::dataContainer();
	vertices->size = (9 * 6) * sizeof(vertex);
	vertices->data = (char*)malloc(vertices->size);

	uint32_t vertexIndex = 0;
	for (int yy = 0; yy < 3; yy++)
    {
		for (int xx = 0; xx < 3; xx++)
		{
			int offset = xx + (yy * 4);

			memcpy(vertices->data + ((vertexIndex + 0) * sizeof(vertex)), tempVertices->data + ((5 + offset) * sizeof(vertex)), sizeof(vertex));
			memcpy(vertices->data + ((vertexIndex + 1) * sizeof(vertex)), tempVertices->data + ((1 + offset) * sizeof(vertex)), sizeof(vertex));
			memcpy(vertices->data + ((vertexIndex + 2) * sizeof(vertex)), tempVertices->data + ((0 + offset) * sizeof(vertex)), sizeof(vertex));
			memcpy(vertices->data + ((vertexIndex + 3) * sizeof(vertex)), tempVertices->data + ((4 + offset) * sizeof(vertex)), sizeof(vertex));
			memcpy(vertices->data + ((vertexIndex + 4) * sizeof(vertex)), tempVertices->data + ((5 + offset) * sizeof(vertex)), sizeof(vertex));
			memcpy(vertices->data + ((vertexIndex + 5) * sizeof(vertex)), tempVertices->data + ((0 + offset) * sizeof(vertex)), sizeof(vertex));

			vertexIndex += 6;
		}
	}

	delete(tempVertices);
	return vertices;
}

#define LAT_DIV 30  
#define LON_DIV 30
#define TOTAL_VERTICES ((LAT_DIV + 1) * (LON_DIV + 1))
#define TOTAL_TRIANGLES (LAT_DIV * LON_DIV * 2)

utils::dataContainer* meshUtility::createSphere()
{
    float radius = 10.0f;
    int vertexIndex = 0;
    int triangleIndex = 0;

    const float PI = 3.14159265358979323846f;

    utils::dataContainer* tempVertices = new utils::dataContainer();
	tempVertices->size = TOTAL_TRIANGLES * 3 * sizeof(vertex);
	tempVertices->data = (char*)malloc(tempVertices->size);

    vertex vertices[TOTAL_VERTICES];
    for (int i = 0; i <= LAT_DIV; ++i) 
    {
        float theta = PI * i / LAT_DIV;
        float v = (float)i / LAT_DIV;    

        for (int j = 0; j <= LON_DIV; ++j) 
        {
            float phi = 2.0f * PI * j / LON_DIV; 
            float u = (float)j / LON_DIV;    

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            vertices[vertexIndex].position.x = x;
            vertices[vertexIndex].position.y = y;
            vertices[vertexIndex].position.z = z;
            vertices[vertexIndex].texcoord.x = u;
            vertices[vertexIndex].texcoord.y = v;
            vertexIndex++;
        }
    }

    char* data_offset = tempVertices->data;
    for (int i = 0; i < LAT_DIV; ++i) 
    {
        for (int j = 0; j < LON_DIV; ++j) 
        {
            int v1 = i * (LON_DIV + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (LON_DIV + 1) + j;
            int v4 = v3 + 1;

            vertices[v4].texcoord.x = 1.0f;
            vertices[v4].texcoord.y = 1.0f;
            vertices[v2].texcoord.x = 1.0f;
            vertices[v2].texcoord.y = 0.0f;
            vertices[v1].texcoord.x = 0.0f;
            vertices[v1].texcoord.y = 0.0f;

            memcpy(data_offset, &vertices[v1], sizeof(vertex));
            data_offset += sizeof(vertex);
            memcpy(data_offset, &vertices[v2], sizeof(vertex));
            data_offset += sizeof(vertex);
            memcpy(data_offset, &vertices[v4], sizeof(vertex));
            data_offset += sizeof(vertex);

            vertices[v3].texcoord.x = 0.0f;
            vertices[v3].texcoord.y = 1.0f;
            vertices[v4].texcoord.x = 1.0f;
            vertices[v4].texcoord.y = 1.0f;
            vertices[v1].texcoord.x = 0.0f;
            vertices[v1].texcoord.y = 0.0f;

            memcpy(data_offset, &vertices[v1], sizeof(vertex));
            data_offset += sizeof(vertex);
            memcpy(data_offset, &vertices[v4], sizeof(vertex));
            data_offset += sizeof(vertex);
            memcpy(data_offset, &vertices[v3], sizeof(vertex));
            data_offset += sizeof(vertex);
        }
    }

   return tempVertices;
}