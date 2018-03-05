#include "labellingtextpreview.h"

#include <QTexture>
#include <QAbstractTexture>
#include <QPlaneMesh>
#include <QSphereMesh>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

LabellingTextPreview::LabellingTextPreview(Qt3DCore::QNode* parent)
    : Qt3DCore::QEntity(parent)
{
    texture = new QTexture2D(this);
    texture->setMinificationFilter(QAbstractTexture::Filter::Linear);
    texture->setMagnificationFilter(QAbstractTexture::Filter::Linear);
    texture->setFormat(QAbstractTexture::TextureFormat::RGBAFormat);

    auto* planeMesh = new Qt3DExtras::QPlaneMesh(this);

    planeTransform = new Qt3DCore::QTransform(this);

    planeMaterial = new Qt3DExtras::QNormalDiffuseMapAlphaMaterial(this);
    planeMaterial->setAmbient(QColor(255, 255, 255, 128));
    planeMaterial->setDiffuse(texture);

    planeEntity = new Qt3DCore::QEntity(this);
    planeEntity->addComponent(planeMesh);
    planeEntity->addComponent(planeMaterial);
    planeEntity->addComponent(planeTransform);

    setText("Enter text");
    setFontName("Arial");
    setTranslation(QVector3D(0, 0, 0));
    setNormal(QVector3D(0, 0, 1));
}
void LabellingTextPreview::setText(QString text)
{
    this->text = text;

    if (textureImage) {
        texture->removeTextureImage(textureImage);
    }

    textureImage = new TextureImage(64, 16, 8.0f, text, fontName);
    textureImage->update();

    texture->addTextureImage(textureImage);
}

void LabellingTextPreview::setFontName(QString fontName)
{
    this->fontName = fontName;

    if (textureImage) {
        texture->removeTextureImage(textureImage);
    }

    textureImage = new TextureImage(64, 16, 8.0f, text, fontName);
    textureImage->update();

    texture->addTextureImage(textureImage);
}

void LabellingTextPreview::setTranslation(const QVector3D& t)
{
    translation = t;

    updateTransform();
}

void LabellingTextPreview::setNormal(const QVector3D& n)
{
    normal = n;

    updateTransform();
}

void LabellingTextPreview::updateTransform()
{
    auto minusNormal= normal;
    minusNormal.setZ(-normal.z());

    auto axis = QVector3D::crossProduct(QVector3D(0, 1, 0), minusNormal);
    auto cos_t = QVector3D::dotProduct(QVector3D(0, 1, 0), minusNormal);
    auto sin_t = sqrtf(1 - cos_t * cos_t);
    auto angle = atan2f(sin_t, cos_t) * 180 / M_PI;

    planeTransform->setTranslation(translation + minusNormal * 0.5f);
    planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 180));
    planeTransform->setScale3D(QVector3D(1, 1.0f, ratioY) * scaleY);
}
