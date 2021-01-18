#pragma once

#include "../Settings/SupportSetting.h"
#include "VerticalSupportModel.h"
#include <array>
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
			std::vector<std::unique_ptr<SupportModel>> createInterconnects(const std::array<SupportModel*, 2>& models);
			std::unique_ptr<SupportModel> createSupport(const OverhangDetect::Overhang& overhang);
			SupportModel* addSupport(std::unique_ptr<SupportModel> target);
			SupportModel* addInterconnect(const std::array<SupportModel*, 2>& pts);

			std::unique_ptr<SupportModel> removeSupport(SupportModel* e);

			bool supportsEmpty()const;
			bool modelHasSupport(const GLModel* model)const;
			
			RaftModel* generateRaft();
			std::unique_ptr<RaftModel> removeRaft();
			RaftModel* addRaft(std::unique_ptr<RaftModel> raft);
			OverhangDetect::Overhangs detectOverhang(const GLModel& model);
			//removed due to efficiency when deleting multiple
			std::vector<const Hix::Render::SceneEntity*> supportModels()const;
			std::vector<SupportModel*> modelAttachedSupports(const std::unordered_set<GLModel*>& models)const;
			std::vector<SupportModel*> interconnects()const;

			RaftModel* raftModel();
			const RaftModel* raftModel()const;

			void clear(const GLModel& model);

			Qt3DCore::QEntity& rootEntity();
			size_t supportCount()const;
			//void checkOverhangCollision(GLModel* model, )
			Hix::Engine3D::RayCaster& supportRaycaster();
			std::vector<std::array<SupportModel*, 2>> interconnectPairs()const;
			void prepareRaycasterSelected();

		private:

			void addToModelMap(SupportModel* support);
			void removeFromModelMap(SupportModel* support);
			void clearImpl(const std::unordered_set<const GLModel*>& models);
			/// <summary>
			/// Set raycaster for all selected models
			/// </summary>
			/// <summary>
			/// Set raycaster for given model
			/// </summary>
			/// <param name="model"></param>
			void prepareRaycaster(const GLModel& model);

			Qt3DCore::QEntity* _root;
			std::vector<QVector3D> getSupportBasePts()const;
			EditMode _supportEditMode = EditMode::Manual;
			std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>> _supports;
			std::unordered_map<const GLModel*, std::unordered_set<ModelAttachedSupport*>> _modelSupportMap;
			std::unique_ptr<RaftModel> _raft;
			//std::unordered_map<const GLModel*, RayCaster> _rayCasters;
			std::unique_ptr<Hix::Engine3D::RayCaster> _rayCaster;
			std::unique_ptr<Hix::Engine3D::RayAccelerator> _rayAccel;

		};

	}
}
