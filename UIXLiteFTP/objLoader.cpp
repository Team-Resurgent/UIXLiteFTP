#include "ObjLoader.h"
#include <algorithm>
#include <iterator>
#include "VectorMath.h"
#include <math.h>

using namespace harmony3d;
using namespace std;

Model * ObjLoader::InstantiateModel(std::string const & path, const float scale)
{
	std::ifstream data(path.c_str());

	if (data.fail()) {
		if (data.is_open())
		{
			data.close();
		}
		return nullptr;
	}

	struct Floats {
		std::vector<float> values;
	};

	struct Face {
		std::vector<int> vertex;
		std::vector<int> texture;
		std::vector<int> normal;
	};

	struct GroupInfo {
		int startOffset;
		int endOffset;
		string groupName;
	};

	std::vector<Floats> vertices;
	std::vector<Floats> texcoords;
	std::vector<Floats> normals;
	std::vector<Face> faces;
	std::vector<GroupInfo> groupInfos;

	GroupInfo groupInfo;
	groupInfo.startOffset = 0;
	groupInfo.endOffset = 0;
	groupInfo.groupName = "modelSurface";

	std::string line, key;
	while (data.good() && !data.eof() && std::getline(data, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;
		if (key == "v") { // vertex
			Floats v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.values.push_back(x);
			}
			vertices.push_back(v);
		}
		else if (key == "vt") { // texture coordinate
			Floats v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.values.push_back(x);
			}
			texcoords.push_back(v);
		}
		else if (key == "vn") { // normal
			Floats v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.values.push_back(x);
			}
			normals.push_back(v);
		}
		else if (key == "usemtl") { // normal
			Floats v; string material; char x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				material += x;
			}
			if (groupInfo.startOffset != groupInfo.endOffset)
			{
				groupInfos.push_back(groupInfo);
				groupInfo.startOffset = (uint32_t)faces.size();
			}
			groupInfo.groupName = material;
		}
		else if (key == "f") { // face
			Face f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v - 1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n - 1);
					}
					else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t - 1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n - 1);
						}
					}
				}
			}
			faces.push_back(f);
			groupInfo.endOffset++;
		}
		else {

		}
	}
	if (groupInfo.startOffset != groupInfo.endOffset)
	{
		groupInfos.push_back(groupInfo);
	}

	VertexGroups vertexGroups;
	VertexGroupInfo vertexGroupInfo;
	vertexGroupInfo.offset = 0;
	vertexGroupInfo.count = 0;

	ShaderMaterialGroups shaderMaterialGroups;
	ShaderMaterialGroupInfo shaderMaterialGroupInfo;
	shaderMaterialGroupInfo.diffuse = Color(255, 255, 255, 255);
	shaderMaterialGroupInfo.shininess = 30.0f;

	for (uint32_t i = 0; i < groupInfos.size(); i++)
	{
		vertexGroupInfo.groupName = groupInfos[i].groupName;
		shaderMaterialGroupInfo.groupName = groupInfos[i].groupName;
		for (uint32_t x = groupInfos[i].startOffset; x < (uint32_t)groupInfos[i].endOffset; x++)
		{
		    const Face face = faces[x];
			int index1 = 0;
			int index2 = -1;
			int index3 = 1;
			for (uint32_t k = 2; k < face.vertex.size(); k++)
			{
				index2 = index3;
				index3 = k;

				Vertex vertex;
				vertex.pos = Vector3(vertices[face.vertex[index1]].values[0], vertices[face.vertex[index1]].values[1], vertices[face.vertex[index1]].values[2]);
				vertex.texCoord = Vector2(texcoords[face.texture[index1]].values[0], texcoords[face.texture[index1]].values[1]);
				vertex.normal = Vector3(normals[face.normal[index1]].values[0], normals[face.normal[index1]].values[1], normals[face.normal[index1]].values[2]);
				vertex.tangent = Vector3();
				vertex.boneIndex = 0;
				vertexGroups.vertices.push_back(vertex);

				vertex.pos = Vector3(vertices[face.vertex[index2]].values[0], vertices[face.vertex[index2]].values[1], vertices[face.vertex[index2]].values[2]);
				vertex.texCoord = Vector2(texcoords[face.texture[index2]].values[0], texcoords[face.texture[index2]].values[1]);
				vertex.normal = Vector3(normals[face.normal[index2]].values[0], normals[face.normal[index2]].values[1], normals[face.normal[index2]].values[2]);
				vertex.tangent = Vector3();
				vertex.boneIndex = 0;
				vertexGroups.vertices.push_back(vertex);

				vertex.pos = Vector3(vertices[face.vertex[index3]].values[0], vertices[face.vertex[index3]].values[1], vertices[face.vertex[index3]].values[2]);
				vertex.texCoord = Vector2(texcoords[face.texture[index3]].values[0], texcoords[face.texture[index3]].values[1]);
				vertex.normal = Vector3(normals[face.normal[index3]].values[0], normals[face.normal[index3]].values[1], normals[face.normal[index3]].values[2]);
				vertex.tangent = Vector3();
				vertex.boneIndex = 0;
				vertexGroups.vertices.push_back(vertex);

				vertexGroupInfo.count+=3;
			}
		}
		vertexGroups.vertexGroupInfos.push_back(vertexGroupInfo);
		vertexGroupInfo.offset += vertexGroupInfo.count;
		vertexGroupInfo.count = 0;
		shaderMaterialGroups.shaderMaterialGroupInfos.push_back(shaderMaterialGroupInfo);
	}

	float minX = vertexGroups.vertices.front().pos.x;
	float maxX = vertexGroups.vertices.front().pos.x;
	float minY = vertexGroups.vertices.front().pos.y;
	float maxY = vertexGroups.vertices.front().pos.y;
	float minZ = vertexGroups.vertices.front().pos.z;
	float maxZ = vertexGroups.vertices.front().pos.z;
	float sumX = 0;
	float sumY = 0;
	float sumZ = 0;

	std::vector<Vertex>::const_iterator const e = vertexGroups.vertices.end();
	for (std::vector<Vertex>::const_iterator i = vertexGroups.vertices.begin(); i != e; ++i)
	{
		Vertex const & v = *i;
		sumX += v.pos.x;
		sumY += v.pos.y;
		sumZ += v.pos.z;
		minX = min(minX, v.pos.x);
		maxX = max(maxX, v.pos.x);
		minY = min(minY, v.pos.y);
		maxY = max(maxY, v.pos.y);
		minZ = min(minZ, v.pos.z);
		maxZ = max(maxZ, v.pos.z);
	}

	float const diffX = maxX - minX;
	float const diffY = maxY - minY;
	float const diffZ = maxZ - minZ;
	float const cenX = maxX - (diffX / 2.f);
	float const cenY = maxY - (diffY / 2.f);
	float const cenZ = maxZ - (diffZ / 2.f);

	float modelScale = (1.f / max(max(diffX, diffY), diffZ)) * scale;

	//Scale and calculate group bounds
	for (uint32_t i = 0; i < (uint32_t)vertexGroups.vertexGroupInfos.size(); i++)
	{
		int startOffset = vertexGroups.vertexGroupInfos[i].offset;
		int endOffset = startOffset + vertexGroups.vertexGroupInfos[i].count;
		for (int v = startOffset; v < endOffset; v++)
		{
			float x = (vertexGroups.vertices[v].pos.x - cenX) * modelScale;
			float y = (vertexGroups.vertices[v].pos.y - cenY) * modelScale;
			float z = (vertexGroups.vertices[v].pos.z - cenZ) * modelScale;
			vertexGroups.vertices[v].pos.x = x;
			vertexGroups.vertices[v].pos.y = y;
			vertexGroups.vertices[v].pos.z = z;
			if (v == startOffset) {
				minX = x;
				maxX = x;
				minY = y;
				maxY = y;
				minZ = z;
				maxZ = z;
			}
			else
			{
				minX = min(minX, x);
				maxX = max(maxX, x);
				minY = min(minY, y);
				maxY = max(maxY, y);
				minZ = min(minZ, z);
				maxZ = max(maxZ, z);
			}
		}
		vertexGroups.vertexGroupInfos[i].bounds = Bounds(minX, maxX, minY, maxY, minZ, maxZ);
	}

	//generateNormals(&vertexGroups);
	generateTangents(&vertexGroups);

	if (data.is_open())
	{
		data.close();
	}

	return new Model(vertexGroups, shaderMaterialGroups);
}

void ObjLoader::generateNormals(VertexGroups *vertexGroups)
{
	uint32_t totalVertices = (uint32_t)vertexGroups->vertices.size();
	for (uint32_t k = 0; k < 2; k++)
	{
		for (uint32_t i = 0; i < (uint32_t)totalVertices; i += 6)
		{
			Vertex *pVertex0 = &vertexGroups->vertices[i];
			Vertex *pVertex1 = &vertexGroups->vertices[i + 1];
			Vertex *pVertex2 = &vertexGroups->vertices[i + 2];
			Vector3 edge1 = pVertex1->pos - pVertex0->pos;
			Vector3 edge2 = pVertex2->pos - pVertex0->pos;
			Vector3 normal = VectorMath::cross(edge1, edge2);

			pVertex0->normal = VectorMath::normal(normal);
			pVertex1->normal = pVertex0->normal;
			pVertex2->normal = pVertex0->normal;
		}

		for (uint32_t i = 3; i < (uint32_t)totalVertices; i += 6)
		{
			Vertex *pVertex0 = &vertexGroups->vertices[i];
			Vertex *pVertex1 = &vertexGroups->vertices[i + 1];
			Vertex *pVertex2 = &vertexGroups->vertices[i + 2];
			Vector3 edge1 = pVertex1->pos - pVertex0->pos;
			Vector3 edge2 = pVertex2->pos - pVertex0->pos;
			Vector3 normal = VectorMath::cross(edge1, edge2);

			pVertex0->normal = VectorMath::normal(normal);
			pVertex1->normal = pVertex0->normal;
			pVertex2->normal = pVertex0->normal;
		}
	}
}

void ObjLoader::generateTangents(VertexGroups *vertexGroups)
{
	Vector3 tangent = Vector3();

	uint32_t totalVertices = (uint32_t)vertexGroups->vertices.size();
	for (uint32_t i = 0; i < (uint32_t)totalVertices; i += 3)
	{
		Vertex *pVertex0 = &vertexGroups->vertices[i];
		Vertex *pVertex1 = &vertexGroups->vertices[i + 1];
		Vertex *pVertex2 = &vertexGroups->vertices[i + 2];
		Vector3 edge1 = pVertex1->pos - pVertex0->pos;
		Vector3 edge2 = pVertex2->pos - pVertex0->pos;
		Vector2 texEdge1 = pVertex1->texCoord - pVertex0->texCoord;
		Vector2 texEdge2 = pVertex2->texCoord - pVertex0->texCoord;
		float det = 1.0f / (texEdge1.x * texEdge2.y - texEdge2.x * texEdge1.y);

		if (fabs(det) < 1e-6f)
		{
			tangent = Vector3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			det = 1.0f / det;
			tangent.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
			tangent.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
			tangent.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;
		}

		pVertex0->tangent = VectorMath::normal(tangent);
		pVertex1->tangent = pVertex0->tangent;
		pVertex2->tangent = pVertex0->tangent;
	}

}