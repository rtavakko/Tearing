#include "openglrendersurface.h"

OpenGLRenderSurface::OpenGLRenderSurface(QScreen *outputScreen,
                                         QObject *parent,
                                         OpenGLRenderer::OpenGLRenderSpecs specs,
                                         const QSurfaceFormat &surfaceFormat,
                                         QOpenGLContext *sharedContext) :
    QOffscreenSurface(outputScreen,parent),
    OpenGLRenderer(specs),
    openGLFormat(surfaceFormat),
    openGLContext(nullptr)
{
    //Create offscreen surface
    setFormat(openGLFormat);
    create();

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

    //Initialize sync timer
    initializeTimer();
    qRegisterMetaType<GLuint>("GLuint");
}

OpenGLRenderSurface::~OpenGLRenderSurface()
{

}

const QSurfaceFormat &OpenGLRenderSurface::getOpenGLFormat()
{
    return openGLFormat;
}

QOpenGLContext *OpenGLRenderSurface::getOpenGLContext()
{
    return openGLContext;
}

void OpenGLRenderSurface::setFrameRate(float fps)
{
    OpenGLRenderer::setFrameRate(fps);
}

void OpenGLRenderSurface::start()
{
    float timeOut = 1000.0f/renderSpecs.frameRate;
    syncTimer->start(timeOut);
}

void OpenGLRenderSurface::stop()
{
    syncTimer->stop();
}

void OpenGLRenderSurface::renderFrame()
{
}

void OpenGLRenderSurface::initializeTimer()
{
    syncTimer = new QTimer(this);
    syncTimer->setTimerType(Qt::PreciseTimer);

    QObject::connect(syncTimer,&QTimer::timeout,this,&OpenGLRenderSurface::renderFrame);
}

void OpenGLRenderSurface::swapSurfaceBuffers()
{
    openGLContext->swapBuffers(this);
}

bool OpenGLRenderSurface::makeContextCurrent()
{
    return openGLContext->makeCurrent(this);
}

void OpenGLRenderSurface::doneContextCurrent()
{
    openGLContext->doneCurrent();
}
