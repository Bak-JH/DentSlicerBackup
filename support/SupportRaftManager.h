#pragma once

#include "DentEngine/src/configuration.h"
#include "VerticalSupportModel.h"
#include "../../common/HetUniquePtr.h"

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
		enum class EditType : uint8_t
		{
			Added = 0,
			Removed = 1
		};
		using namespace Engine3D;
		class SupportModel;
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

			void autoGen(const GLModel& model, SlicingConfiguration::SupportType supType);
			void addSupport(const OverhangDetect::Overhang& overhang);
			void removeSupport(SupportModel* e);
			void applyEdits();
			void cancelEdits();
			void generateSupport(const Hix::OverhangDetect::Overhangs& overhangs);
			void generateRaft();
			OverhangDetect::Overhangs detectOverhang(const GLModel& model);
			//removed due to efficiency when deleting multiple
			std::vector<std::reference_wrapper<const Hix::Render::SceneEntity>> supportModels()const;
			const Hix::Render::SceneEntity* raftModel()const;
			void clear();
			void clear(const GLModel& model);

			Qt3DCore::QEntity& rootEntity();
			size_t supportCount()const;
			//void checkOverhangCollision(GLModel* model, )
			RayCaster& supportRaycaster();

		private:
			void clearImpl(const std::unordered_set<const GLModel*>& models);
			void prepareRaycaster(const GLModel& model);
			Qt3DCore::QEntity _root;
			std::vector<QVector3D> getSupportBasePts()const;
			bool _supportExist = false;
			bool _raftExist = false;
			std::unordered_map<SupportModel*, EditType> _pendingSupports;
			EditMode _supportEditMode = EditMode::None;
			SlicingConfiguration::SupportType _supportType;
			std::unordered_set<Hix::Memory::HetUniquePtr<SupportModel>> _supports;
			std::unique_ptr<RaftModel> _raft;
			std::unique_ptr<RayCaster> _rayCaster;
		};

	}
}
