#pragma once
#include <qquickitem.h>
#include <qqmlcomponent.h>
#include <unordered_set>
#include "../render/Bounds3D.h"
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
			bool unlistModel(GLModel* model); // return false = there is something wrong
			bool isListed(GLModel* model)const;
			bool isSelected(GLModel* model)const;
			void unselectAll();
			void selectAll();
			//returns if select changed
			bool setModelSelected(GLModel* model, bool isSelected);
			bool setContiguousSelected(GLModel* model);
			std::unordered_set<GLModel*> selectedModels()const;
			Hix::Engine3D::Bounds3D selectedBound()const;
			void deleteSelectedModels();
		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;

			Hix::QML::Controls::Button* _deleteButton;
			std::unordered_set<GLModel*> _selectedModels;
			std::unordered_map<GLModel*, std::unique_ptr<PartListItem>> _items;
			QQuickItem* _itemContainer;
			QQmlComponent _component;
		};


		class PartListItem : public QQuickItem, public ControlOwner
		{
			Q_OBJECT
			Q_PROPERTY(QString modelName MEMBER _modelName NOTIFY nameChanged)
		public:
			PartListItem(QQuickItem* parent = nullptr);
			virtual ~PartListItem();
			void setSelected(bool isSelected);
			void setTargetModel(GLModel* model);
			GLModel* targetModel();
			bool visible()const;
			Hix::QML::Controls::ToggleSwitch* hideButton();
			Hix::QML::Controls::Button* selectButton();

		signals:
			void nameChanged();

		protected:
			void componentComplete() override;
			QQuickItem* getQItem()override;
			void onPositionChanged();

			Hix::QML::Controls::ToggleSwitch* _hideButton;
			Hix::QML::Controls::Button* _selectButton;
			QString _modelName;
			bool _selected = false;

		private:
			GLModel* _targetModel;
		};
	}
}