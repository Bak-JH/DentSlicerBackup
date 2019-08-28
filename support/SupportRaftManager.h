#pragma once

#include "../DentEngine/src/configuration.h"
#include "VerticalSupportModel.h"
#include <memory>
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
		class SupportRaftManager
		{
		public:
			SupportRaftManager(GLModel* parent);
			~SupportRaftManager();
			bool supportActive()const;
			bool raftActive()const;
			EditMode supportEditMode()const;
			void  setSupportEditMode(EditMode mode);
			float supportRaftMinLength()const;
			//coordinate for bottom of support and raft
			float raftBottom()const;
			float supportBottom()const;

			void generateSuppAndRaft(SlicingConfiguration::SupportType supType, SlicingConfiguration::RaftType raftType);
			void addSupport(const std::variant<VertexConstItr, FaceOverhang>& supportSpec);
			void removeSupport(SupportModel* e);
			void applyEdits();
			void cancelEdits();
			void generateSupport();
			void generateRaft();
			void clear();
			GLModel* getModel();

			//void attachToModel(RaftModel* support);

		private:
			GLModel* _owner;
			bool _supportExist = false;
			bool _raftExist = false;
			std::unordered_map<SupportModel*, EditType> _pendingSupports;
			EditMode _supportEditMode = EditMode::None;
			SlicingConfiguration::SupportType _supportType;
			SlicingConfiguration::RaftType _raftType;
			std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>> _supports;
			std::unique_ptr<RaftModel> _raft;
			Hix::OverhangDetect::Overhangs _overhangs;
		};

	}
}
