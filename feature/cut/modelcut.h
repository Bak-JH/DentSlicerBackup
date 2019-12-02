#pragma once
#include "../interfaces/Feature.h"
#include "../../DentEngine/src/ContourBuilder.h"
#include "../../DentEngine/src/Bounds3D.h"
#include "DrawingPlane.h"
namespace Hix
{
	namespace Features
	{
		class ModelCut : public Feature
		{
		public:
			enum CutType
			{
				ZAxial,
				Polyline
			};
			ModelCut(const std::unordered_set<GLModel*>& selectedModels, Hix::Engine3D::Bounds3D bound);
			virtual ~ModelCut();
			void cutModeSelected(int type);
			void getSliderSignal(double value);
			void applyCut();
			void undo() override {}
				
			// polyline
			void addCuttingPoint(QVector3D v);
			void removeCuttingPoint(int idx);
			void removeCuttingPoints();
			void drawLine(QVector3D endpoint);

		private:
			//cutting
			CutType _cutType = ZAxial;
			const Engine3D::Mesh* _origMesh;
			Hix::Features::Cut::DrawingPlane _cuttingPlane;
			Hix::Engine3D::Bounds3D _modelsBound;
			std::unordered_set<GLModel*> _models;

			std::deque<GLModel*> _modelHistory;
		};
	}
}
