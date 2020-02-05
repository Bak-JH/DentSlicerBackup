#pragma once
#include <qquickitem.h>
#include <unordered_set>
#include "../DentEngine/src/Bounds3D.h"

class GLModel;

namespace Hix
{
	namespace QML
	{

		namespace Controls
		{
			class Button;
		}

		class PartList : public QQuickItem
		{
			Q_OBJECT
		public:
			PartList(QQuickItem* parent = nullptr);
			virtual ~PartList();
			void listModel(GLModel* model);
			bool isSelected(GLModel* model)const;
			void selectPart(GLModel* model);
			void unselectPart(GLModel* model);
			std::unordered_set<GLModel*> selectedModels()const;
			Hix::Engine3D::Bounds3D selectedBound()const;

		protected:
			void deleteSelectedModels();
			void componentComplete() override;
			Hix::QML::Controls::Button* _deleteButton;
			std::unordered_set<GLModel*> _selectedModels;
		};

		class PartListItem : public QQuickItem
		{
			Q_OBJECT
		public:
			PartListItem(QQuickItem* parent = nullptr);
			virtual ~PartListItem();
		protected:
			void componentComplete() override;
			GLModel* _model = nullptr;
			bool _selected = false;
		};
	}
}