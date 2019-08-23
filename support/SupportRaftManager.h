#pragma once

#include "../DentEngine/src/configuration.h"
#include "VerticalSupportModel.h"
#include <memory>
class GLModel;
namespace Hix
{
	namespace Support
	{
		enum class SupportEditMode : uint8_t
		{
			None = 0,
			Delete = 1,
			Add = 2
		};
		using namespace Engine3D;
		class SupportModel;
		class SupportRaftManager
		{
		public:
			SupportRaftManager(GLModel* parent);
			~SupportRaftManager();
			bool supportActive()const;
			bool raftActive()const;
			SupportEditMode supportEditMode()const;
			void  setSupportEditMode(SupportEditMode mode);
			float supportRaftMinLength()const;
			//coordinate for bottom of support and raft
			float supportRaftBottom()const;

			void generateSuppAndRaft(SlicingConfiguration::SupportType supType, SlicingConfiguration::RaftType raftType);
			void addSupport(const std::variant<VertexConstItr, FaceOverhang>& supportSpec);
			void removeSupport(const std::variant<VertexConstItr, FaceOverhang>& supportSpec);
			void clear();

			//void attachToModel(RaftModel* support);

		private:
			GLModel* _owner;
			bool _supportExist = false;
			bool _raftExist = false;
			SupportEditMode _supportEditMode;
			SlicingConfiguration::SupportType _supportType;
			SlicingConfiguration::RaftType _raftType;
			std::unordered_map<QVector3D, std::unique_ptr<SupportModel>> _faceSupports;
			std::unordered_map<VertexConstItr, std::unique_ptr<SupportModel>> _vertexSupports;
		};

	}
}
