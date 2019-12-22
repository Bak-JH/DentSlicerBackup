#include "autoarrange.h"
#include"../../glmodel.h"
#include "SkylineBinPack.h"
#include <unordered_map>
#include <unordered_set>
#include "../../DentEngine/src/polyclipping/polyclipping.h"
#include "../../qmlmanager.h"

constexpr float MARGIN = 20;
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

void translateFromBottLeft(GLModel* model, float xTranslate, float yTranslate)
{
	auto bound = model->recursiveAabb();
	auto currTranslation = model->transform().translation();
	QVector3D translation(-bound.xMin() + xTranslate, -bound.yMin() + yTranslate,0);
	model->moveModel(translation);
}
Hix::Features::Autoarrange::Autoarrange(const std::unordered_set<GLModel*>& selected)
{
	std::unordered_multimap<rbp::RectSize, GLModel*> modelRectMap;
	std::unordered_set<GLModel*> widthLonger;
	std::vector<rbp::RectSize> rectsInput;
	std::vector<rbp::Rect> packedOutput;
	for (auto model : selected)
	{
		auto bound = model->recursiveAabb();
		int x = bound.lengthX() * Hix::Polyclipping::INT_PT_RESOLUTION + MARGIN;
		int y = bound.lengthY() * Hix::Polyclipping::INT_PT_RESOLUTION + MARGIN;
		if (x < y)
		{
			std::swap(x, y);
		}
		else
		{
			widthLonger.insert(model);
		}
		rbp::RectSize rect{x,y};
		rectsInput.push_back(rect);
		modelRectMap.insert(std::make_pair(rect, model));
	}
	rbp::SkylineBinPack skylinePacker;
	skylinePacker.Init(qmlManager->settings().printerSetting.bedX * Hix::Polyclipping::INT_PT_RESOLUTION, qmlManager->settings().printerSetting.bedY* Hix::Polyclipping::INT_PT_RESOLUTION, false);
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
	int xCenter = xMax/2;
	int yCenter = yMax/2;

	for (auto& each : packedOutput)
	{
		bool isRectWidthLonger = true;
		bool isModelWidthLonger = true;
		rbp::RectSize lookupRect { each.width, each.height };
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
		translateFromBottLeft(model, xTranslate, yTranslate);
		modelRectMap.erase(equalRange.first);
	}
}
