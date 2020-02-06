#pragma once
#include <qquickitem.h>
#include <unordered_set>
#include "../DentEngine/src/Bounds3D.h"
#include "ControlOwner.h"
class GLModel;

namespace Hix
{
	namespace QML
	{

		namespace Controls
		{
			class Button;
		}
		class PartListItem;
		class PartList : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
		public:
			PartList(QQuickItem* parent = nullptr);
			virtual ~PartList();
			void listModel(GLModel* model);
			bool isListed(GLModel* model)const;
			bool isSelected(GLModel* model)const;
			void setModelSelected(GLModel* model, bool isSelected);
			std::unordered_set<GLModel*> selectedModels()const;
			Hix::Engine3D::Bounds3D selectedBound()const;

		protected:
			void deleteSelectedModels();
			void componentComplete() override;
			QQuickItem* getQItem()override;

			Hix::QML::Controls::Button* _deleteButton;
			std::unordered_set<GLModel*> _selectedModels;
			std::unordered_map<GLModel*, std::unique_ptr<PartListItem>> _items;
			friend class PartListItemAttorny;
		};


		class PartListItem : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
		public:
			PartListItem(QQuickItem* parent = nullptr);
			virtual ~PartListItem();
			void setSelected(bool isSelected);
			bool isSelected()const;
			bool visible()const;
		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;

			Hix::QML::Controls::ToggleSwitch* _hideButton;
			Hix::QML::Controls::ToggleSwitch* _selectButton;
			GLModel* _model = nullptr;
			PartList* _parent = nullptr;
			bool _selected = false;
		};
	}
}