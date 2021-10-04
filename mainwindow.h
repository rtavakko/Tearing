#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>

#include <QBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDial>

#include <trianglerenderer.h>
#include <invertfilter.h>

#include <opengldisplay.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,
                        QScreen* uiScreen = nullptr,
                        QScreen* outputScreen = nullptr,
                        OpenGLRenderer::OpenGLRenderSpecs specs = OpenGLRenderer::OpenGLRenderSpecs{
            OpenGLRenderer::OpenGLTextureSpecs{
            640,
            360,
            4,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE
            },
            60.0
            },
            unsigned int numDisplayWindows = 1);

    ~MainWindow();

signals:
    void setRenderFPS(double fps);
    void screenChanged(QScreen* screen);

private:
    bool initialize();

    bool initializeUI();

    Ui::MainWindow *ui;

    //For central widget
    QBoxLayout* mainLayout;
    QWidget* mainWidget;

    //Other widgets
    QLineEdit* textFPS;

    QLabel* lActualRenderFPS;

    QPushButton* pbSetRenderFPS;

    QPushButton* pbStartVideo;
    QPushButton* pbStopVideo;

    //Render specs
    OpenGLRenderer::OpenGLRenderSpecs videoSpecs;
    QScreen* mainUIScreen;
    QScreen* mainOutputScreen;

    //Renders a texture on a separate thread
    TriangleRenderer* triangleRenderer;

    //Invert filter; takes triangle texture and inverts its colors
    InvertFilter* invertFilter;

    QThread* renderThread;
    QThread* filterThread;

    //Displays the texture rendered by textureRenderer on the GUI thread
    unsigned int numDisplays;
    std::vector<OpenGLDisplay*> displays;

    //Used for timing
    std::chrono::time_point<std::chrono::high_resolution_clock> t_startRender;
    std::chrono::time_point<std::chrono::high_resolution_clock> t_endRender;

    std::chrono::duration<double,std::milli> t_deltaRender;

    double textRefreshTime;
};

#endif // MAINWINDOW_H
