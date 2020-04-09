#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <qvector3d.h>
#include "../DentEngine/src/Bounds3D.h"
class GLModel;
class QQuickItem;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace QML
	{
		class PartList;
		class PrintInfo;
	}
	namespace Features
	{
		class ListModel;
	}
	namespace Application
	{
		class ApplicationManager;
		class PartManager
		{
		public:
			PartManager();
			~PartManager();
			PartManager(const PartManager& other) = delete;
			PartManager(PartManager&& other) = delete;
			PartManager& operator=(PartManager other) = delete;
			PartManager& operator=(PartManager&& other) = delete;

			void addPart(std::unique_ptr<GLModel>&& model);
			std::unique_ptr<GLModel> removePart(GLModel* model);
			//checks if given model is top level AKA listed model pre-tree
			bool isTopLevel(GLModel* model)const;
			bool isSelected(GLModel* model)const;
			void setSelected(GLModel* model, bool selected);
			void selectAll();
			void unselectAll();
			void deleteSelectedModels();
			std::unordered_set<GLModel*> selectedModels()const;
			std::unordered_set<GLModel*> allModels()const;
			Hix::Engine3D::Bounds3D selectedBound()const;
			QVector3D getSelectedCenter()const;
			QVector3D selectedModelsLengths()const;
			Qt3DCore::QEntity* modelRoot();
			void setMultiSelect(bool isMulti);
			void setContiguousSelect(bool isMulti);
			bool isMultiSelect()const;
			bool isContiguousSelect()const;
		private:
			Qt3DCore::QEntity* _root;
			std::unordered_map<GLModel*, std::unique_ptr<GLModel>> _models;
			Hix::QML::PartList* _partList;
			bool _isMulti = false;
			bool _isContiguous= false;
			friend class PartManagerLoader;
		};

		class PartManagerLoader
		{
		private:
			static void init(PartManager& manager, QObject* root);
			friend class Hix::Application::ApplicationManager;
		};





	}
}
