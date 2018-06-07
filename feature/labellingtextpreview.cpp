#include "feature/labellingtextpreview.h"

#include <QTexture>
#include <QAbstractTexture>
#include <QPlaneMesh>
#include <QSphereMesh>
#include <cmath>
#include <QDebug>

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

    setFontName("Arial");
    setFontBold(false);
    setFontSize(12);
    setTranslation(QVector3D(0, 0, 0));
    setNormal(QVector3D(0, 0, 1));
    setText("Enter text", 64);
}
void LabellingTextPreview::setText(QString text, int contentWidth)
{
    this->text = text;
    this->contentWidth = contentWidth;

    if (textureImage) {
        texture->removeTextureImage(textureImage);
    }

    width = this->contentWidth*2;// * this->fontSize;
    qDebug() << "text : " << text << "contentWidth: " << contentWidth << "width : " << width;
    if (width < minimumWidth)
        width = minimumWidth;

    textureImage = new TextureImage(width, this->fontSize/(1+1.1*log(this->fontSize/12)), 8.0f, text, fontName, fontWeight);
    textureImage->update();

    texture->addTextureImage(textureImage);

    updateTransform();
}

void LabellingTextPreview::setFontName(QString fontName)
{
    this->fontName = fontName;
}

void LabellingTextPreview::setFontBold(bool isbold){
    if (isbold){
        qDebug() << "set font bold";
        this->fontWeight = QFont::Bold;
    } else {
        this->fontWeight = QFont::Normal;
    }
}

void LabellingTextPreview::setFontSize(int fontSize)
{
    scaleY = fontSize*scaleY/this->fontSize;

    this->fontSize = fontSize;
}

void LabellingTextPreview::setTranslation(const QVector3D& t)
{
    translation = t;

    updateTransform();
}

void LabellingTextPreview::setNormal(const QVector3D& n)
{
    normal = n;
    normal.normalize();

    updateTransform();
}

void LabellingTextPreview::updateTransform()
{
    QVector3D ref = QVector3D(0, 0, 1);
    auto tangent = QVector3D::crossProduct(normal, ref);
    tangent.normalize();
    auto binormal = QVector3D::crossProduct(tangent, normal);
    binormal.normalize();
    qDebug() << QVector3D(width / minimumWidth, 1.0f, ratioY) * scaleY << scaleY;
    planeTransform->setTranslation(translation + normal * 0.5f);
    planeTransform->setRotation(QQuaternion::fromAxes(tangent, normal, binormal) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180));
    planeTransform->setScale3D(QVector3D(width / minimumWidth, 2.0f, ratioY) * scaleY);
}
