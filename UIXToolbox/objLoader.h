#ifndef __Harmony3d__ObjLoader__
#define __Harmony3d__ObjLoader__

#include <istream>
#include <stdint.h>
#include "Model.h"

namespace harmony3d
{
    class ObjLoader
    {
    public:
        static uint32_t const k_InitialVerticeReservation = 2048;
		static Model * InstantiateModel(std::string const & path, float const a_Scale);
		static void generateNormals(VertexGroups *vertexGroups);
		static void generateTangents(VertexGroups *vertexGroups);
    };
}

#endif
