#pragma once

#include "mesh.h"
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
	using namespace Hix::Engine3D;
	template<>
	struct hash<std::pair<VertexConstItr, VertexConstItr>>
	{
		//2D only!
		std::size_t operator()(const std::pair<VertexConstItr, VertexConstItr>& pair)const
		{
			//we don't care about symmertricity as the vertice pair should be sorted anyway ie) no symmetry
			return std::hash<VertexConstItr>()(pair.first) ^ std::hash<VertexConstItr>()(pair.second);
		}
	};
}



namespace Hix
{
	namespace Slicer
	{
		using namespace Hix::Engine3D;
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
			FaceConstItr face;
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
			ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z);
			std::vector<Contour> buildContours();
			std::vector<Contour> flushIncompleteContours();
			static bool isArea(const Contour& contour);


		private:
			//could use bool, just incase we need to resolve non-2-maifold

			//two points

			void buildSegment(const FaceConstItr& mf);
			bool buildContour(Contour& current, bool reverse);
			QVector2D planeIntersectionPt(VertexConstItr vtxA0, VertexConstItr vtxA1);
			std::vector<Contour>  joinOrCloseIncompleteContours();

			const Mesh* _mesh;
			float _plane;
			std::unordered_map<std::pair<VertexConstItr, VertexConstItr>, QVector2D> _midPtLUT;
			const std::unordered_set<FaceConstItr>& _intersectList;
			std::unordered_map<FaceConstItr, ContourSegment> _segments;

			std::unordered_multimap<QVector2D, ContourSegment*> _fromHash;
			std::unordered_multimap<QVector2D, ContourSegment*> _toHash;

			std::unordered_set<FaceConstItr> _unexplored;
			std::vector<Contour> _incompleteContours;

		};
	}
}
