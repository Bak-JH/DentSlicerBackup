#include "feature/labellingtextpreview.h"

#include <QTexture>
#include <QAbstractTexture>
#include <QPlaneMesh>
#include <QSphereMesh>
#include <cmath>

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

    setText("Enter text", QString("Enter text").size());
    setFontName("Arial");
    setTranslation(QVector3D(0, 0, 0));
    setNormal(QVector3D(0, 0, 1));
}
void LabellingTextPreview::setText(QString text, int contentWidth)
{
    this->text = text;

    if (textureImage) {
        texture->removeTextureImage(textureImage);
    }

    width = contentWidth * 2;
    qDebug() << width;

    if (width < minimumWidth)
        width = minimumWidth;

    textureImage = new TextureImage(width, 16, 8.0f, text, fontName);
    textureImage->update();

    texture->addTextureImage(textureImage);

    updateTransform();
}

void LabellingTextPreview::setFontName(QString fontName)
{
    this->fontName = fontName;

    if (textureImage) {
        texture->removeTextureImage(textureImage);
    }

    width = text.size() * 12;

    if (width < minimumWidth)
        width = minimumWidth;

    textureImage = new TextureImage(width, 16, 8.0f, text, fontName);
    textureImage->update();

    texture->addTextureImage(textureImage);

    updateTransform();
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
    QMatrix3x3 first = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), 180).toRotationMatrix();
    float first_angle = (normal[1] == 0) ? 180 :-atanf(normal[2]/normal[1])*180/M_PI;
    QMatrix3x3 second = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), first_angle).toRotationMatrix();
    float second_angle = (normal[1] == 0) ? 180 :atanf(normal[0]/normal[1])*180/M_PI;
    QMatrix3x3 third = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), second_angle).toRotationMatrix();

    /*float first_angle = (normal[2] == 0) ? 180 : atanf(normal[0]/normal[2]);
    float second_angle = (normal[0] == 0) ? 180 : atanf(normal[1]/normal[0]);
    QMatrix3x3 first = QQuaternion::fromAxisAndAngle(QVector3D(1,1,0), first_angle).toRotationMatrix();
    QMatrix3x3 second = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), second_angle).toRotationMatrix();*/
    /*auto axis = QVector3D::crossProduct(QVector3D(1,0,0), -normal);
    axis.normalize();
    auto cos_t = QVector3D::dotProduct(QVector3D(1,0,0), -normal);

    auto sin_t = sqrtf(1 - cos_t * cos_t);
    auto angle = atan2f(cos_t, sin_t) * 180 / M_PI;
    */
    planeTransform->setTranslation(translation + normal * 0.5f);
    //planeTransform->setRotation(QQuaternion::fromAxisAndAngle(axis, angle + 180));
    //planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), 180));
    planeTransform->setRotation(QQuaternion::fromRotationMatrix(first*second*third));
    //planeTransform->setRotation(QQuaternion::fromRotationMatrix(first*second));
    planeTransform->setScale3D(QVector3D(width / minimumWidth, 1.0f, ratioY) * scaleY);
}
