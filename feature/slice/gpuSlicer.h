#pragma once
#include <vector>
#include "../common/Semaphore.h"
#include <future>
#include <unordered_set>
#include <filesystem>
#include <memory>
#include "../render/Bounds3D.h"

struct GLFWwindow;
class Shader;
class GLModel;

namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}
	namespace Support
	{
		class SupportRaftManager;
	}
	namespace Slicer
	{
		struct LayerInfo
		{
			float height;
			size_t AAZ;
		};
		class SlicerGL
		{
		public:
			SlicerGL(float delta, std::filesystem::path outPath, size_t sampleXY, size_t sampleZ, float minHeight);
			~SlicerGL();
			void setScreen(float pixelWidth, size_t imgX, size_t imgY);
			void setBounds(const Hix::Engine3D::Bounds3D& bound);
			size_t run(const std::vector<float>& vtcs);
		private:
			void setFrameBuffer(bool isMultiSampleEnable);
			void addVtcs(const std::vector<float>& vtcs);
			void glfwSlice(Shader& shader, float height, size_t index);

			void setUniforms(Shader& shader, float height, float maxBright);
			void writeToFile(const std::vector<uint8_t>& data, size_t index);
			size_t _concurrentWriteMax;
			float _layer = 0.1;
			unsigned int  _VAO, _vertVBO, _sliceFBO, _sliceRBO, _stencilRBO, _maskVAO, _maskVBO, _blittedFBO, _blittedRBO, _blitStencilRBO;
			size_t _vertCnt;
			Hix::Engine3D::Bounds3D _bounds;

			//pitch of pixel
			float _pixel = 0.05;
			//size of sliced image in mm(or other units);
			float _imgX;
			float _imgY;
			float _minHeight;
			size_t _resX;
			size_t _resY;
			size_t _sampleXY = 1;
			size_t _sampleZ = 1;
			GLFWwindow* _window = nullptr;
			Hix::Common::Concurrency::Semaphore _fileWriteSem;
			std::vector<uint8_t> _singlePassBuffer;
			std::vector <std::vector<uint8_t>> _finalBuffers;
			std::vector<std::future<void>> _pendingWrites;
			std::filesystem::path _outPath;

		};
	}
}


