#ifndef OPENGLNATIVERENDERWINDOW_H
#define OPENGLNATIVERENDERWINDOW_H

#include <openglrenderer.h>

#include <QOffScreenSurface>
#include <QOpenGLContext>

#include <QtPlatformHeaders/QWGLNativeContext>
#include <WinUser.h>
#include <wingdi.h>
#include <windef.h>

#include <errhandlingapi.h>

class OpenGLNativeRenderWindow : public QOffscreenSurface, public OpenGLRenderer
{
    Q_OBJECT
public:
    OpenGLNativeRenderWindow(QScreen* outputScreen,
                             OpenGLRenderer::OpenGLRenderSpecs specs,
                             const QSurfaceFormat& surfaceFormat,
                             QOpenGLContext* sharedContext);

    static LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void displayLastError();

    const QSurfaceFormat& getOpenGLFormat();
    QOpenGLContext* getOpenGLContext();

    HWND getWindowHandle() const;
    HDC getWindowRenderContext() const;
    HGLRC getOpenGLContextHandle() const;

    void setOpenGLContextHandle(HGLRC handle);

    bool isVisible() const;

public slots:

    virtual void createNative();

    virtual void showNative();

    virtual void resize(unsigned int w, unsigned int h) override;
    virtual void updateSpecs(OpenGLRenderer::OpenGLRenderSpecs specs) override;

    virtual void renderFrame() override;

    void setFrame(GLuint texID, unsigned int width, unsigned int height);

signals:
    void renderedFrame(double actualFPS);

protected:

    void swapSurfaceBuffers();

    bool makeContextCurrent();
    void doneContextCurrent();

    void swapSurfaceBuffersNative();

    bool makeContextCurrentNative();
    void doneContextCurrentNative();

    QSurfaceFormat openGLFormat;
    QOpenGLContext* openGLContext;

    QOpenGLContext* sharedOpenGLContext;

    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;

    GLuint inputTextureID;

    bool visible;
};

#endif // OPENGLNATIVERENDERWINDOW_H
