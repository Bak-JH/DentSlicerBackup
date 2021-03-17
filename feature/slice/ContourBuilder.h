#pragma once

#include "../../Mesh/mesh.h"
#include <QVector2D>




namespace std
{

	template<>
	struct hash<QVector2D>
	{
		//2D only!
		std::size_t operator()(const QVector2D& pt)const
		{
			size_t x = (size_t)(pt.x() * Hix::Polyclipping::INT_PT_RESOLUTION);
			size_t y = (size_t)(pt.y() * Hix::Polyclipping::INT_PT_RESOLUTION);
			size_t digest = x | y << 32;
			return digest;
		}
	};
	template<>
	struct hash<std::pair<Hix::Engine3D::VertexConstItr, Hix::Engine3D::VertexConstItr>>
	{
		//2D only!
		std::size_t operator()(const std::pair<Hix::Engine3D::VertexConstItr, Hix::Engine3D::VertexConstItr>& pair)const
		{
			//we don't care about symmertricity as the vertice pair should be sorted anyway ie) no symmetry
			return std::hash<Hix::Engine3D::VertexConstItr>()(pair.first) ^ std::hash<Hix::Engine3D::VertexConstItr>()(pair.second);
		}
	};
}



namespace Hix
{
	namespace Slicer
	{
		struct ContourSegment
		{
			enum FlipResult : uint8_t
			{
				NotFlipped = 0,
				Flipped = 1,
				UnknownDirection = 2
			};
			ContourSegment();
			//constructor helper;
			bool isValid()const;
			float dist()const;
			Hix::Engine3D::FaceConstItr face;
			QVector2D from;
			QVector2D to;
		};

		class Contour
		{
		public:
			//Contour(const ContourSegment* start);
			bool isClosed()const;
			//attempts to close the unclosed contour, if the gap is too large, give up.
			bool tryClose();
			//IntPoint getDestination();
			void addNext(const ContourSegment& seg);
			void addPrev(const ContourSegment& seg);
			float dist()const;
			QVector2D& from();
			QVector2D& to();
			void append(const Contour& appended);
			bool isOutward()const;
			ClipperLib::Path toPath()const;//tmp
			ClipperLib::Path toPath(std::vector<QVector2D>& outFloatPath)const;//tmp
			ClipperLib::Path toDebugPath()const;//tmp
			std::deque<ContourSegment> segments;
		};


		//one per plane
		class ContourBuilder
		{
		public:
			ContourBuilder(const Hix::Engine3D::Mesh* mesh, std::unordered_set<Hix::Engine3D::FaceConstItr>& intersectingFaces, float z);
			std::vector<Contour> buildContours();
			std::vector<Contour> flushIncompleteContours();
			static bool isArea(const Contour& contour);


		private:
			//could use bool, just incase we need to resolve non-2-maifold

			//two points

			void buildSegment(const Hix::Engine3D::FaceConstItr& mf);
			bool buildContour(Contour& current, bool reverse);
			QVector2D planeIntersectionPt(Hix::Engine3D::VertexConstItr vtxA0, Hix::Engine3D::VertexConstItr vtxA1);
			std::vector<Contour>  joinOrCloseIncompleteContours();

			const Hix::Engine3D::Mesh* _mesh;
			float _plane;
			std::unordered_map<std::pair<Hix::Engine3D::VertexConstItr, Hix::Engine3D::VertexConstItr>, QVector2D> _midPtLUT;
			const std::unordered_set<Hix::Engine3D::FaceConstItr>& _intersectList;
			std::unordered_map<Hix::Engine3D::FaceConstItr, ContourSegment> _segments;

			std::unordered_multimap<QVector2D, ContourSegment*> _fromHash;
			std::unordered_multimap<QVector2D, ContourSegment*> _toHash;

			std::unordered_set<Hix::Engine3D::FaceConstItr> _unexplored;
			std::vector<Contour> _incompleteContours;

		};
	}
}
