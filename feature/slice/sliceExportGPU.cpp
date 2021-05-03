#include "../slice/gpuSlicer.h"

#include "sliceExportGPU.h"
#include "../glmodel.h"
#include "../addModel.h"
#include "../application/ApplicationManager.h"
#include "../Qml/components/Inputs.h"
#include "../Qml/components/Buttons.h"
#include "../slice/InfoWriter.h"

#include "../slice/slicingengine.h"
#include "../Mesh/mesh.h"
#include "../render/SceneEntity.h"
#include "../glmodel.h"
#include "../Qt/QtUtils.h"
#include "../zip/zip.h"

#include <QFileDialog>
#include <QtCore/QCoreApplication>
#include <QDebug>

#include <unordered_set>
#include <filesystem>
constexpr float ZMARGIN = 5;
using namespace Hix::Settings;

using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Render;
using namespace Hix::Slicer;
using namespace Hix::QtUtils;



Hix::Features::SliceExportGPU::SliceExportGPU(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path)
{
}





void Hix::Features::SliceExportGPU::run()
{
    //don't throw
    std::error_code error;
    //tmp directory
    auto tmpPath = std::filesystem::temp_directory_path() / "tmpSlice";
    std::filesystem::remove_all(tmpPath, error);
    std::filesystem::create_directory(tmpPath, error);

	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;
	auto& printerSetting = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;

	// Export to SVG
	Hix::Slicer::SlicerGL slicer(setting.layerHeight, tmpPath, setting.AAXY, setting.AAZ, setting.minHeight);
	slicer.setScreen(printerSetting.pixelSizeX(), printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY);
    Hix::Engine3D::Bounds3D bounds;
    auto vtcs = toVtxBuffer(bounds);
    slicer.setBounds(bounds);
	auto layerCnt = slicer.run(vtcs);
	//write info files
	Hix::Slicer::InfoWriter iw(tmpPath, printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY, setting.layerHeight);
	iw.createInfoFile();
	iw.writeBasicInfo(layerCnt, printerSetting.printerConstants);
	
	if (printerSetting.infoFileType == Hix::Settings::PrinterSetting::InfoFileType::ThreeDelight)
	{
		iw.writeVittroOptions(layerCnt, printerSetting.bedBound);
	}

    //save to zip
    miniz_cpp::zip_file file;

    for (const auto& entry : std::filesystem::directory_iterator(tmpPath)) {
        if (entry.is_regular_file()) {
            auto filePath = entry.path();
            file.write(filePath.string(), filePath.filename().string());
        }
    }
    file.comment = "hix slice file";
    std::ofstream zipOut(toStdPath(_path), std::ios_base::trunc | std::ios::binary);
    file.save(zipOut);
}


void genVertexBuffer(float xOffset, float yOffset, bool xInverted, std::vector<float>& buffer, const Hix::Engine3D::Mesh& mesh, Hix::Engine3D::Bounds3D& bound)
{
    auto& faces = mesh.getFaces();
    if (xInverted)
    {
        for (auto itr = faces.cbegin(); itr != faces.cend(); ++itr)
        {

            auto mvs = itr.meshVertices();

            for (int j = 2; j >= 0; --j)
            {
                auto mv = mvs[j].worldPosition();
                QVector3D pt{ -mv.x() + xOffset, mv.y() + yOffset, mv.z() };
                bound.update(pt);
                buffer.emplace_back(pt.x());
                buffer.emplace_back(pt.y());
                buffer.emplace_back(pt.z());
            }
        }
    }
    else
    {
        for (auto itr = faces.cbegin(); itr != faces.cend(); ++itr)
        {

            auto mvs = itr.meshVertices();
            for (int j = 0; j < 3; ++j)
            {
                auto pt = mvs[j].worldPosition();
                bound.update(pt);
                buffer.emplace_back(pt.x() + xOffset);
                buffer.emplace_back(pt.y() + yOffset);
                buffer.emplace_back(pt.z());
            }
        }
    }
}

std::vector<float> Hix::Features::SliceExportGPU::toVtxBuffer(Hix::Engine3D::Bounds3D& bounds)
{
    std::vector<float> vtcs;

    std::unordered_set<const SceneEntity*> entities = SlicingEngine::selectedToEntities(_models, Hix::Application::ApplicationManager::getInstance().supportRaftManager());
    std::unordered_set<const SceneEntity*> modelsAndChildren;
    size_t vtxCnt = 0;
    for (auto m : entities)
    {
        m->SceneEntity::getChildrenModels(modelsAndChildren);
        modelsAndChildren.insert(m);
    }
    for (auto& m : modelsAndChildren)
    {
        vtxCnt += m->getMesh()->getVertices().size();
    }
    auto& printerSetting = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
    auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;

    ;
    vtcs.reserve(vtxCnt);
    for (auto& m : modelsAndChildren)
    {
        genVertexBuffer(printerSetting.bedOffsetX, printerSetting.bedOffsetY, printerSetting.invertX , vtcs, *m->getMesh(), bounds);
    }
    return vtcs;
}

