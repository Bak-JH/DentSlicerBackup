#pragma once
#include <QVector3D>
#include "Mesh/mesh.h"
#include <QDebug>

QVector3D PtOnTri(const QVector3D point, Hix::Engine3D::FaceConstItr face)
{
	auto meshVertex = face.meshVertices();
	auto diff = meshVertex[0].worldPosition() - point;
	auto edge0 = meshVertex[1].worldPosition() - meshVertex[0].worldPosition();
	auto edge1 = meshVertex[2].worldPosition() - meshVertex[0].worldPosition();
	auto a00 = QVector3D::dotProduct(edge0, edge0);
	auto a01 = QVector3D::dotProduct(edge0, edge1);
	auto a11 = QVector3D::dotProduct(edge1, edge1);
	auto b0 = QVector3D::dotProduct(diff, edge0);
	auto b1 = QVector3D::dotProduct(diff, edge1);
	auto det = std::abs(a00 * a11 - a01 * a01);
	auto s = a01 * b1 - a11 * b0;
	auto t = a01 * b0 - a00 * b1;

	if (s + t <= det)
	{
		if (s < 0)
		{
			if (t < 0) // region 4
			{
				if (b0 < 0)
				{
					t = 0;
					if (-b0 >= a00)
					{
						// VN1
						s = 1;
					}
					else
					{
						// EN0
						s = -b0 / a00;
					}
				}
				else
				{
					s = 0;

					if (b1 >= 0)
					{
						// VN0
						t = 0;
					}
					else if (-b1 >= a11)
					{
						// VN2
						t = 1;
					}
					else
					{
						// EN2
						t = -b1 / a11;
					}
				}
			}
			else // region 3
			{
				s = 0;

				if (b1 >= 0)
				{
					// VN0
					t = 0;
				}
				else if (-b1 >= a11)
				{
					// VN2
					t = 1;
				}
				else
				{
					// EN2
					t = -b1 / a11;
				}
			}
		}
		else if (t < 0) // region 5
		{
			t = 0;

			if (b0 >= 0)
			{
				// VN0
				s = 0;
			}
			else if (-b0 >= a00)
			{
				// VN1
				s = 1;
			}
			else
			{
				// EN0
				s = -b0 / a00;
			}
		}
		else // region 0
		{
			// FN
			// minimum at interior point
			auto invDet = (1) / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		double tmp0, tmp1, numer, denom;

		if (s < 0) // region 2
		{
			tmp0 = a01 + b0;
			tmp1 = a11 + b1;

			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a00 - (2) * a01 + a11;

				if (numer >= denom)
				{
					// VN1
					s = 1;
					t = 0;
				}
				else
				{
					// EN1
					s = numer / denom;
					t = 1 - s;
				}
			}
			else
			{
				s = 0;

				if (tmp1 <= 0)
				{
					// VN2
					t = 1;
				}
				else if (b1 >= 0)
				{
					// VN0
					t = 0;
				}
				else
				{
					// EN2
					t = -b1 / a11;
				}
			}
		}
		else if (t < 0) // region 6
		{
			tmp0 = a01 + b1;
			tmp1 = a00 + b0;

			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a00 - 2 * a01 + a11;

				if (numer >= denom)
				{
					// VN2
					t = 1;
					s = 0;
				}
				else
				{
					// EN1
					t = numer / denom;
					s = 1 - t;
				}
			}
			else
			{
				t = 0;

				if (tmp1 <= 0)
				{
					// VN1
					s = 1;
				}
				else if (b0 >= 0)
				{
					// VN0
					s = 0;
				}
				else
				{
					// EN0
					s = -b0 / a00;
				}
			}
		}
		else // region 1
		{
			numer = a11 + b1 - a01 - b0;

			if (numer <= 0)
			{
				// VN2
				s = 0;
				t = 1;
			}
			else
			{
				denom = a00 - (2) * a01 + a11;

				if (numer >= denom)
				{
					// VN1
					s = 1;
					t = 0;
				}
				else
				{
					// EN1
					s = numer / denom;
					t = 1 - s;
				}
			}
		}
	}

	auto nearestVertex = meshVertex[0].localPosition() + edge0 * s + edge1 * t;
	return nearestVertex;
}



//void writeToFile(TrackedIndexedList<Hix::Engine3D::MeshVertex, std::allocator<Hix::Engine3D::MeshVertex>, Hix::Engine3D::VertexItrFactory>& vertices, int index)
//{
//	std::stringstream idxStream;
//	idxStream << index;
//	auto idxStr = idxStream.str() + ".png";
//	std::filesystem::path file = (idxStr);
//	stbi_write_png(file.c_str(), 1000, 1000, 1, vertices.cbegin().ref(), 1000);
//}
