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
			FlipResult calcNormalAndFlip();
			float dist()const;
			void flip();
			//
			//ordering is important.
			//follows Righ hand thumb finger rule, if the in, goint to->from normal vector is pointed left side, CW 
			FaceConstItr face;
			QVector2D from;
			QVector2D to;
			QVector2D normal;
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
			//void calculateDirection();
			//bool isOutward();
			std::deque<ContourSegment> segments;
			ClipperLib::Path toPath()const;//tmp
			ClipperLib::Path toPath(std::vector<QVector2D>& outFloatPath)const;//tmp

		private:
			////bounds
			//cInt _xMax = std::numeric_limits<cInt>::lowest();
			//cInt _xMin = std::numeric_limits<cInt>::max();
			//cInt _yMax = std::numeric_limits<cInt>::lowest();
			//cInt _yMin = std::numeric_limits<cInt>::max();
				//bool _isOutward = false;
			bool _directionDetermined = false;
		};


		//one per plane
		class ContourBuilder
		{
		public:
			ContourBuilder(const Mesh* mesh, std::unordered_set<FaceConstItr>& intersectingFaces, float z);
			std::vector<Contour> buildContours();
			std::vector<Contour> flushIncompleteContours();


		private:
			//could use bool, just incase we need to resolve non-2-maifold

			//two points

			bool isArea(const Contour& contour);
			void buildSegment(const FaceConstItr& mf);
			bool buildContour(Contour& current, bool reverse);
			QVector2D midPoint2D(VertexConstItr vtxA0, VertexConstItr vtxA1);
			std::unordered_set<Contour*> joinOrCloseIncompleteContours();

			const Mesh* _mesh;
			float _plane;
			std::unordered_map<std::pair<VertexConstItr, VertexConstItr>, QVector2D> _midPtLUT;
			const std::unordered_set<FaceConstItr>& _intersectList;
			std::unordered_map<FaceConstItr, ContourSegment> _segments;

			std::unordered_multimap<QVector2D, ContourSegment*> _fromHash;
			std::unordered_multimap<QVector2D, ContourSegment*> _toHash;
			std::unordered_multimap<QVector2D, ContourSegment*> _unknownHash;
			std::unordered_set<ContourSegment*> _unknownSegs;

			std::unordered_set<FaceConstItr> _unexplored;
			std::vector<Contour> _incompleteContours;

		};
	}
}
