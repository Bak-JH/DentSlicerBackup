#include "autoarrange.h"
#include"../../glmodel.h"
#include "SkylineBinPack.h"
#include <unordered_map>
#include <unordered_set>
#include "../../DentEngine/src/polyclipping/polyclipping.h"

#include "../move.h"
#include "../../application/ApplicationManager.h"
#include "../../common/RandomGen.h"
using namespace Hix::Features;
using namespace Hix::Engine3D;
constexpr float MARGIN = 1;
namespace std
{

	template<>
	struct hash<rbp::RectSize>
	{
		//2D only!
		std::size_t operator()(const rbp::RectSize& pt)const
		{
			size_t x = (size_t)(pt.width);
			size_t y = (size_t)(pt.height);
			size_t digest = x | y << 32;
			return digest;
		}
	};
}

Hix::Features::AutoArrangeMode::AutoArrangeMode()
{
	auto work = new AutoArrange(Hix::Application::ApplicationManager::getInstance().partManager().allModels());
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(work);
}

Hix::Features::AutoArrangeMode::~AutoArrangeMode()
{
}


Hix::Features::Move* translateFromBottLeft(GLModel* model, float xTranslate, float yTranslate)
{
	auto bound = model->recursiveAabb();
	auto currTranslation = model->transform().translation();
	QVector3D translation(-bound.xMin() + xTranslate, -bound.yMin() + yTranslate,0);
	return new Move(model, translation);
}
Hix::Features::AutoArrange::AutoArrange(const std::unordered_set<GLModel*>& selected): _selected(selected)
{
	_progress.setDisplayText("Arrange Models");
}

void Hix::Features::AutoArrange::runImpl()
{
	std::unordered_multimap<rbp::RectSize, GLModel*> modelRectMap;
	std::unordered_set<GLModel*> widthLonger;
	std::vector<rbp::RectSize> rectsInput;
	std::vector<rbp::Rect> packedOutput;
	for (auto model : _selected)
	{
		auto bound = model->recursiveAabb();
		int x = (bound.lengthX() + MARGIN) * Hix::Polyclipping::INT_PT_RESOLUTION;
		int y = (bound.lengthY() + MARGIN) * Hix::Polyclipping::INT_PT_RESOLUTION;
		if (x < y)
		{
			std::swap(x, y);
		}
		else
		{
			widthLonger.insert(model);
		}
		rbp::RectSize rect{ x,y };
		rectsInput.push_back(rect);
		modelRectMap.insert(std::make_pair(rect, model));
	}
	rbp::SkylineBinPack skylinePacker;
	skylinePacker.Init(Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthX() * Hix::Polyclipping::INT_PT_RESOLUTION, Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthY() * Hix::Polyclipping::INT_PT_RESOLUTION, false);
	skylinePacker.Insert(rectsInput, packedOutput, rbp::SkylineBinPack::LevelBottomLeft);
	//calculate max occupied rect so that we can re-center
	int xMax = 0;
	int yMax = 0;
	for (auto& each : packedOutput)
	{
		auto x = each.x + each.width;
		auto y = each.y + each.height;
		if (xMax < x)
			xMax = x;
		if (yMax < y)
			yMax = y;
	}
	int xCenter = xMax / 2;
	int yCenter = yMax / 2;

	for (auto& each : packedOutput)
	{
		bool isRectWidthLonger = true;
		bool isModelWidthLonger = true;
		rbp::RectSize lookupRect{ each.width, each.height };
		if (lookupRect.width < lookupRect.height)
		{
			std::swap(lookupRect.width, lookupRect.height);
			isRectWidthLonger = false;
		}
		auto equalRange = modelRectMap.equal_range(lookupRect);
		auto model = equalRange.first->second;
		if (widthLonger.find(model) == widthLonger.end())
		{
			isModelWidthLonger = false;
		}
		if (isRectWidthLonger != isModelWidthLonger)
		{
			//requires rotation
			auto rotation = QQuaternion::fromEulerAngles(0, 0, 90);
			model->rotateModel(rotation);
			model->rotateDone();
		}
		float xTranslate = double(each.x - xCenter) / Hix::Polyclipping::INT_PT_RESOLUTION;
		float yTranslate = double(each.y - yCenter) / Hix::Polyclipping::INT_PT_RESOLUTION;
		addFeature(translateFromBottLeft(model, xTranslate, yTranslate));
		modelRectMap.erase(equalRange.first);
	}
	FeatureContainer::runImpl();
}

Hix::Features::AutoArrangeAppend::AutoArrangeAppend(GLModel* model): Move(model)
{
}

void Hix::Features::AutoArrangeAppend::runImpl()
{
	//calculate inner fit polygon range
	_to = QVector3D(0, 0, 0);
	auto bound = _model->recursiveAabb();
	const auto& printBound = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound;
	if (!printBound.contains(bound))
	{
		//model is too large
		//throw std::runtime_error("model is too large for arrange");
		__super::runImpl();
		return;
	}
	auto maxDisp = printBound.calculateMaxDisplacement(bound);
	RandomGen random(0);
	auto allModels = Hix::Application::ApplicationManager::getInstance().partManager().allModels();
	std::vector<Bounds3D> modelBounds;
	modelBounds.reserve(allModels.size());
	for (auto& m : allModels)
	{
		if(m != _model)
			modelBounds.emplace_back(m->recursiveAabb());
	}
	for (float mult = 0.01f; mult <= 1.0f; mult += 0.01f)
	{
		bool intersects = false;
		float xMin = maxDisp[0] * mult;
		float xMax = maxDisp[1] * mult;
		float yMin = maxDisp[2] * mult;
		float yMax = maxDisp[3] * mult;

		for (size_t i = 0; i < 6000; ++i)
		{
			QVector3D disp(random.getFloat(xMin, xMax), random.getFloat(yMin, yMax), 0);
			auto curr = bound;
			curr.translate(disp);
			for (auto& b : modelBounds)
			{
				if (b.intersects2D(curr))
				{
					intersects = true;
					break;
				}
			}
			if (!intersects)
			{
				_to = disp;
				break;
				qDebug() << i << mult;
			}
		}
		if (!intersects)
		{
			break;
		}
	}
	__super::runImpl();

}
