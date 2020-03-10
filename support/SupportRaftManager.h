#pragma once

#include "../Settings/SupportSetting.h"
#include "VerticalSupportModel.h"

class GLModel;
namespace Hix
{
	namespace Support
	{
		enum class EditMode : uint8_t
		{
			None = 0,
			Manual = 1
		};

		using namespace Engine3D;
		class SupportModel;
		class ModelAttachedSupport;
		class RaftModel;
		class SupportRaftManager : public QObject
		{
		public:
			static float raftBottom();
			static float supportBottom();
			static float supportRaftMinLength();

			SupportRaftManager();
			void initialize(Qt3DCore::QEntity* parent);
			~SupportRaftManager();
			bool supportActive()const;
			bool raftActive()const;
			EditMode supportEditMode()const;
			void  setSupportEditMode(EditMode mode);
			//coordinate for bottom of support and raft

			SupportModel* addSupport(const OverhangDetect::Overhang& overhang);
			SupportModel* addSupport(std::unique_ptr<SupportModel> target);
			std::unique_ptr<SupportModel> removeSupport(SupportModel* e);

			bool supportsEmpty()const;
			bool modelHasSupport(const GLModel* model)const;
			
			RaftModel* generateRaft();
			std::unique_ptr<RaftModel> removeRaft();
			RaftModel* addRaft(std::unique_ptr<RaftModel> raft);
			OverhangDetect::Overhangs detectOverhang(const GLModel& model);
			//removed due to efficiency when deleting multiple
			std::vector<std::reference_wrapper<const Hix::Render::SceneEntity>> supportModels()const;
			std::vector<SupportModel*> modelAttachedSupports(const std::unordered_set<GLModel*>& models)const;
			const Hix::Render::SceneEntity* raftModel()const;
			void clear(const GLModel& model);

			Qt3DCore::QEntity& rootEntity();
			size_t supportCount()const;
			//void checkOverhangCollision(GLModel* model, )
			RayCaster& supportRaycaster();

		private:

			void addToModelMap(SupportModel* support);
			void removeFromModelMap(SupportModel* support);
			void clearImpl(const std::unordered_set<const GLModel*>& models);
			void prepareRaycaster(const GLModel& model);
			Qt3DCore::QEntity* _root;
			std::vector<QVector3D> getSupportBasePts()const;
			EditMode _supportEditMode = EditMode::None;
			std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>> _supports;
			std::unordered_map<const GLModel*, std::unordered_set<ModelAttachedSupport*>> _modelSupportMap;
			std::unique_ptr<RaftModel> _raft;
			std::unique_ptr<RayCaster> _rayCaster;

		};

	}
}
