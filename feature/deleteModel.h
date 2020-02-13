#pragma once
#include "feature/interfaces/Feature.h"
#include "feature/interfaces/FlushSupport.h"
#include <QObject>
#include <Qt3DCore>
#include <variant>
#include <memory>
namespace Hix
{
	namespace Features
	{
		class DeleteModelMode : public Mode
		{
		public:
			DeleteModelMode();
			virtual ~DeleteModelMode();
		};
		class DeleteModel : public Feature, public FlushSupport
		{
		public:
			DeleteModel(GLModel* target);
			virtual ~DeleteModel();
			GLModel* getDeletedModel();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			struct RedoInfo
			{
				std::unique_ptr<GLModel> redoModel;
				Qt3DCore::QNode* parent;
				bool isListed;

			};
			std::variant<GLModel*, RedoInfo> _model;
		};
	}
}                                                                                