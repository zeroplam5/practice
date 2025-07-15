#pragma once

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | \
                           aiProcess_GenSmoothNormals | \
                           aiProcess_FlipUVs | \
                           aiProcess_JoinIdenticalVertices | \
                           aiProcess_ConvertToLeftHanded)
