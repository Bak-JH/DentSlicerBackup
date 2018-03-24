#ifndef LABELLINGTEXTPREVIEW_H
#define LABELLINGTEXTPREVIEW_H

#include <QEntity>
#include <Qt3DCore/QTransform>
#include <QNormalDiffuseMapAlphaMaterial>

#include <QPaintedTextureImage>
#include <QPainter>
#include <QFont>

class LabellingTextPreview : public Qt3DCore::QEntity
{
    Q_OBJECT

    class TextureImage : public Qt3DRender::QPaintedTextureImage {
        friend class LabellingTextPreview;
    public:
        TextureImage(int w, int h, float scale, QString text, QString fontName)
            : scale(scale), text(text), fontName(fontName)
        {
            setSize(QSize(w * scale, h * scale));
        }

    protected:
        void paint(QPainter *painter)
        {
            int w = painter->device()->width();
            int h = painter->device()->height();

            painter->eraseRect(0, 0, w, h);
            painter->fillRect(0, 0, w, h, QColor(255, 255, 255, 128));
            painter->setPen(QColor(0, 0, 0));
            painter->setFont(QFont(fontName, 12 * scale, QFont::Normal, false));
            painter->drawText(QRectF(0, 0, w, h), text, Qt::AlignHCenter | Qt::AlignVCenter);
        }

    private:
        float scale;
        QString text;
        QString fontName;
    };

public:
    LabellingTextPreview(Qt3DCore::QNode* parent = nullptr);

    void setText(QString text, int contentWidth);
    void setFontName(QString fontName);
    void setTranslation(const QVector3D& t);
    void setNormal(const QVector3D& n);

    void updateTransform();

    QVector3D translation;
    QVector3D normal;
    QString text;
    QString fontName = "Arial";

    float minimumScaleX = 1.0f;
    float minimumWidth = 64;
    float width = 64;
    float scaleY = 5.0f;
    float ratioY = 16.0f / 64;

private:

    Qt3DCore::QEntity* planeEntity = nullptr;
    Qt3DCore::QTransform* planeTransform = nullptr;
    Qt3DExtras::QNormalDiffuseMapAlphaMaterial* planeMaterial = nullptr;

    Qt3DRender::QAbstractTexture* texture = nullptr;
    TextureImage* textureImage = nullptr;
};

#endif // LABELLINGTEXTPREVIEW_H