#include "openglrenderwindow.h"

OpenGLRenderWindow::OpenGLRenderWindow(QScreen *outputScreen,
                                       OpenGLRenderer::OpenGLRenderSpecs specs,
                                       const QSurfaceFormat &surfaceFormat,
                                       QOpenGLContext *sharedContext) :
    QWindow(outputScreen),
    OpenGLRenderer(specs),
    openGLFormat(surfaceFormat),
    openGLContext(nullptr)
{
    //Create offscreen surface
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(openGLFormat);
    create();
    //show();

    //Allocate memory for the context object and prepare to create it
    openGLContext = new QOpenGLContext(this);
    openGLContext->setFormat(openGLFormat);
    if(sharedContext)
        openGLContext->setShareContext(sharedContext);

    //Make sure the context is created & is sharing resources with the shared context
    bool contextCreated = openGLContext->create();
    assert(contextCreated);

    if(sharedContext)
    {
        bool sharing = QOpenGLContext::areSharing(openGLContext,sharedContext);
        assert(sharing);
    }
}

OpenGLRenderWindow::~OpenGLRenderWindow()
{

}

const QSurfaceFormat &OpenGLRenderWindow::getOpenGLFormat()
{
    return openGLFormat;
}

QOpenGLContext *OpenGLRenderWindow::getOpenGLContext()
{
    return openGLContext;
}

void OpenGLRenderWindow::swapSurfaceBuffers()
{
    openGLContext->swapBuffers(this);
}

bool OpenGLRenderWindow::makeContextCurrent()
{
    return openGLContext->makeCurrent(this);
}

void OpenGLRenderWindow::doneContextCurrent()
{
    openGLContext->doneCurrent();
}
