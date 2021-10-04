#ifndef OPENGLRENDERWINDOW_H
#define OPENGLRENDERWINDOW_H

#include <openglrenderer.h>

#include <QWindow>
#include <QOpenGLContext>

class OpenGLRenderWindow : public QWindow, public OpenGLRenderer
{
    Q_OBJECT
public:
    OpenGLRenderWindow(QScreen* outputScreen,
                       OpenGLRenderer::OpenGLRenderSpecs specs,
                       const QSurfaceFormat& surfaceFormat,
                       QOpenGLContext* sharedContext);

    virtual ~OpenGLRenderWindow();

    const QSurfaceFormat& getOpenGLFormat();
    QOpenGLContext* getOpenGLContext();

protected:
    void swapSurfaceBuffers();

    bool makeContextCurrent();
    void doneContextCurrent();

    QSurfaceFormat openGLFormat;
    QOpenGLContext* openGLContext;
};

#endif // OPENGLRENDERWINDOW_H
