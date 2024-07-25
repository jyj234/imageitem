#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QObject>
#include <QMutex>
#include <QImage>
#include <QQuickPaintedItem>
#include <QQuickFramebufferObject>
#include <QQmlEngine>

class ImageItemRenderer;

class ImageItem: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_DISABLE_COPY( ImageItem )
    QML_NAMED_ELEMENT(ImageItem)
    Q_PROPERTY(QImage image READ getImage WRITE setImage NOTIFY imageChanged);
public:
    ImageItem();

    ~ImageItem() override;

public:
    Q_INVOKABLE void setImage(const QImage &image);
    Q_INVOKABLE const QImage& getImage();
    Q_INVOKABLE void clearImage(bool istransparent = true);
signals:
    void imageChanged();
private:
    Renderer *createRenderer() const override;

private:
    QImage m_image;
};

#endif // IMAGEITEM_H
