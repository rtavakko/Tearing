#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>

#include <openglrendersurface.h>
#include <opengldisplay.h>

class VideoThread : public QThread
{
public:
    VideoThread(OpenGLRenderSurface* source);

    OpenGLDisplay* getDisplay();
public slots:

    virtual void resizeOutput(unsigned int w, unsigned int h);
    virtual void updateSpecs(OpenGLRenderer::OpenGLRenderSpecs specs);

    void setFrame(GLuint texID, unsigned int width, unsigned int height);

protected:
    virtual void run() override;

    bool createOutput();

    bool outputInitialized;

    OpenGLRenderSurface* renderer;

    OpenGLDisplay* display;
};

#endif // VIDEOTHREAD_H
