#pragma once
#include <QVector3D>
#include "Mesh/mesh.h"

QVector3D PtOnTri(const QVector3D point, std::array<Hix::Engine3D::ConstItrInfo::VertexItrType, 3> face)
{
	auto diff = face[0].worldPosition() - point;
	auto edge0 = face[1].worldPosition() - face[0].worldPosition();
	auto edge1 = face[2].worldPosition() - face[0].worldPosition();
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

	auto nearestVertex = face[0].worldPosition() + edge0 * s + edge1 * t;

	return nearestVertex;
}


QVector3D getClosestPoint(Triangle triangle, QVector3D point)
{
	Plane plane = new Plane(triangle.p0, triangle.p1, triangle.p2);
	point = ClosestPoint(plane, point);

	if (PointInTriangle(triangle, point)) {
		return new Point(point);
	}

	Line AB = new Line(triangle.p0, triangle.p1);
	Line BC = new Line(triangle.p1, triangle.p2);
	Line CA = new Line(triangle.p2, triangle.p0);

	QVector3D c1 = ClosestPoint(AB, point);
	QVector3D c2 = ClosestPoint(BC, point);
	QVector3D c3 = ClosestPoint(CA, point);

	float mag1 = (point.ToVector() - c1.ToVector()).LengthSquared();
	float mag2 = (point.ToVector() - c2.ToVector()).LengthSquared();
	float mag3 = (point.ToVector() - c3.ToVector()).LengthSquared();

	float min = Math.Min(mag1, mag2);
	min = Math.Min(min, mag3);

	if (min == mag1) {
		return c1;
	}
	else if (min == mag2) {
		return c2;
	}
	return c3;
}

//
//
//void writeToFile(TrackedIndexedList<Hix::Engine3D::MeshVertex, std::allocator<Hix::Engine3D::MeshVertex>, Hix::Engine3D::VertexItrFactory>& vertices, int index)
//{
//	std::stringstream idxStream;
//	idxStream << index;
//	auto idxStr = idxStream.str() + ".png";
//	std::filesystem::path file = (idxStr);
//	stbi_write_png(file.c_str(), 1000, 1000, 1, vertices.cbegin().ref(), 1000);
//}

void drawbmp(const char* filename, TrackedIndexedList<Hix::Engine3D::MeshVertex, std::allocator<Hix::Engine3D::MeshVertex>, Hix::Engine3D::VertexItrFactory>& vertices,
			float width, float height) {

	unsigned int headers[13];
	FILE* outfile;
	int extrabytes;
	int paddedsize;
	int x; int y; int n;
	int red, green, blue;

	extrabytes = 4 - (((int)width * 3) % 4);                 // How many bytes of padding to add to each
														// horizontal line - the size of which must
														// be a multiple of 4 bytes.
	if (extrabytes == 4)
		extrabytes = 0;

	paddedsize = (((int)width * 3) + extrabytes) * (int)height;

	// Headers...
	// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".

	headers[0] = paddedsize + 54;      // bfSize (whole file size)
	headers[1] = 0;                    // bfReserved (both)
	headers[2] = 54;                   // bfOffbits
	headers[3] = 40;                   // biSize
	headers[4] = 2560;  // biWidth
	headers[5] = 1620; // biHeight

	// Would have biPlanes and biBitCount in position 6, but they're shorts.
	// It's easier to write them out separately (see below) than pretend
	// they're a single int, especially with endian issues...

	headers[7] = 0;                    // biCompression
	headers[8] = paddedsize;           // biSizeImage
	headers[9] = 0;                    // biXPelsPerMeter
	headers[10] = 0;                    // biYPelsPerMeter
	headers[11] = 0;                    // biClrUsed
	headers[12] = 0;                    // biClrImportant

	outfile = fopen(filename, "wb");

	//
	// Headers begin...
	// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
	//

	fprintf(outfile, "BM");

	for (n = 0; n <= 5; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	// These next 4 characters are for the biPlanes and biBitCount fields.

	fprintf(outfile, "%c", 1);
	fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);
	fprintf(outfile, "%c", 0);

	for (n = 7; n <= 12; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	//
	// Headers done, now write the data...
	//

	for (y = (int)height - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
	{
		for (x = 0; x <= (int)width - 1; x++)
		{

			red = 255;
			green = 255;
			blue = 255;

			// Also, it's written in (b,g,r) format...

			fprintf(outfile, "%c", blue);
			fprintf(outfile, "%c", green);
			fprintf(outfile, "%c", red);
		}
		if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
		{
			for (n = 1; n <= extrabytes; n++)
			{
				fprintf(outfile, "%c", 0);
			}
		}
	}

	fclose(outfile);
	return;
}
