#ifndef OPENGLDISPLAY_H
#define OPENGLDISPLAY_H

#include <openglrenderwindow.h>;

#include <QResizeEvent>
#include <QBackingStore>
#include <QPainter>

class OpenGLDisplay : public OpenGLRenderWindow
{
    Q_OBJECT
public:
    OpenGLDisplay(QScreen* outputScreen,
                  OpenGLRenderer::OpenGLRenderSpecs specs,
                  const QSurfaceFormat& surfaceFormat,
                  QOpenGLContext* sharedContext);

    ~OpenGLDisplay();

    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent* event) override;

    void resizeEvent(QResizeEvent *event) override;

    virtual void lockSize();
    virtual void unlockSize();

public slots:

    virtual void resize(unsigned int w, unsigned int h) override;
    virtual void updateSpecs(OpenGLRenderer::OpenGLRenderSpecs specs) override;

    virtual void renderFrame() override;

    void setFrame(GLuint texID, unsigned int width, unsigned int height);

    void renderLater();

signals:
    void renderedFrame(double actualFPS);

protected:
    GLuint inputTextureID;
};

#endif // OPENGLDISPLAY_H
