#pragma once

// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2019
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 3.24.0 (2019/04/08)


#include <Mathematics/GteDistPoint3Plane3.h>
#include <Mathematics/GteEdgeKey.h>
#include <map>

// The algorithm for splitting a mesh by a plane is described in
// https://www.geometrictools.com/Documentation/ClipMesh.pdf
// Currently, the code here does not include generating a closed
// mesh (from the "positive" and "zero" vertices) by attaching
// triangulated faces to the mesh, where the those faces live in
// the splitting plane.  (TODO: Add this code.)

namespace Hix
{
	namespace Features
	{
		namespace Cut
		{
			class MeshPlaneBisect
			{

			public:
			//	void operator();

			//private:
			//	void ClassifyVertices(std::vector<Vector3<Real>> const& clipVertices, Plane3<Real> const& plane);
			//	void ClassifyEdges(std::vector<Vector3<Real>>& clipVertices, std::vector<int> const& indices);
			//	void ClassifyTriangles(std::vector<int> const& indices,
			//		std::vector<int>& negIndices, std::vector<int>& posIndices);
			//	void AppendTriangle(std::vector<int>& indices, int v0, int v1, int v2)
			//	{
			//		indices.push_back(v0);
			//		indices.push_back(v1);
			//		indices.push_back(v2);
			//	}
			//	void SplitTrianglePPM(std::vector<int>& negIndices,
			//		std::vector<int>& posIndices, int v0, int v1, int v2);

			//	void SplitTriangleMMP(std::vector<int>& negIndices,
			//		std::vector<int>& posIndices, int v0, int v1, int v2);

			//	void SplitTrianglePMZ(std::vector<int>& negIndices,
			//		std::vector<int>& posIndices, int v0, int v1, int v2);

			//	void SplitTriangleMPZ(std::vector<int>& negIndices,
			//		std::vector<int>& posIndices, int v0, int v1, int v2);

			//	// Stores the signed distances from the vertices to the plane.
			//	std::vector<Real> mSignedDistances;

			//	// Stores the edges whose vertices are on opposite sides of the
			//	// plane.  The key is a pair of indices into the vertex array.
			//	// The value is the point of intersection of the edge with the
			//	// plane and an index into m_kVertices (the index is larger or
			//	// equal to the number of vertices of incoming rkVertices).
			//	std::map<EdgeKey<false>, std::pair<Vector3<Real>, int>> mEMap;
			};

		}
	}
}
