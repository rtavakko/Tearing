#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent,
                       QScreen* uiScreen,
                       QScreen *outputScreen,
                       OpenGLRenderer::OpenGLRenderSpecs specs,
                       unsigned int numDisplayWindows) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mainWidget(new QWidget(this)),
    mainLayout(new QBoxLayout(QBoxLayout::Direction::TopToBottom)),
    textFPS(nullptr),
    lActualRenderFPS(nullptr),
    pbSetRenderFPS(nullptr),
    pbStartVideo(nullptr),
    pbStopVideo(nullptr),
    videoSpecs(specs),
    mainUIScreen(uiScreen),
    mainOutputScreen(outputScreen),
    triangleRenderer(nullptr),
    invertFilter(nullptr),
    renderThread(nullptr),
    filterThread(nullptr),
    numDisplays(numDisplayWindows),
    previewDisplay(nullptr),
    videoRenderThread(nullptr),
    textRefreshTime(150)
{
    ui->setupUi(this);

    bool initialized = initialize();
    //bool initialized = initializeThreaded();
    assert(initialized);

    bool uiInitialized = initializeUI();
    assert(uiInitialized);

    renderThread->start();
    filterThread->start();
    //videoRenderThread->start();
}

MainWindow::~MainWindow()
{
    renderThread->quit();

    delete ui;
}

bool MainWindow::initialize()
{
    //Renderer
    renderThread = new QThread();
    filterThread = new QThread();
    triangleRenderer = new TriangleRenderer(mainOutputScreen,
                                            nullptr,
                                            videoSpecs,
                                            QSurfaceFormat::defaultFormat(),
                                            nullptr);

    invertFilter = new InvertFilter(mainOutputScreen,
                                    nullptr,
                                    videoSpecs,
                                    QSurfaceFormat::defaultFormat(),
                                    triangleRenderer->getOpenGLContext());

    triangleRenderer->moveToThread(renderThread);
    invertFilter->moveToThread(renderThread);

    QObject::connect(this,&MainWindow::setRenderFPS,triangleRenderer,&OpenGLRenderSurface::setFrameRate);

    QObject::connect(triangleRenderer,&TriangleRenderer::frameReady,invertFilter,&InvertFilter::processFrame);

    QObject::connect(renderThread,&QThread::started,triangleRenderer,&OpenGLRenderSurface::start);
    QObject::connect(renderThread,&QThread::finished,this,[=]()
    {
        delete triangleRenderer;
        triangleRenderer = nullptr;

        foreach(OpenGLDisplay* display, textureDisplay)
        {
            delete display;
            display = nullptr;
        }
    });

    //Displays
    for(unsigned int i = 0; i < numDisplays; i++)
    {
        OpenGLDisplay* display = new OpenGLDisplay(mainOutputScreen,
                                                   videoSpecs,
                                                   QSurfaceFormat::defaultFormat(),
                                                   invertFilter->getOpenGLContext());
        QObject::connect(invertFilter,&OpenGLRenderSurface::frameReady,display,&OpenGLDisplay::setFrame);
        display->show();

        //Update the size of every display except for the first one
        if(i != 0)
            QObject::connect(this,&MainWindow::screenChanged,display,&OpenGLDisplay::setScreen);

        textureDisplay.push_back(display);
    }

    QObject::connect(this,&MainWindow::screenChanged,triangleRenderer,&OpenGLRenderSurface::setScreen);
    //The first display causes size update signal to fire to update all the other displays
    if(textureDisplay.size() != 0)
        QObject::connect(textureDisplay[0],&OpenGLDisplay::screenChanged,this,[=](QScreen* screen)
        {
            emit screenChanged(screen);
        });

    return true;
}

bool MainWindow::initializeThreaded()
{
    //Renderer
    triangleRenderer = new TriangleRenderer(mainOutputScreen,
                                            nullptr,
                                            videoSpecs,
                                            QSurfaceFormat::defaultFormat(),
                                            nullptr);
    videoRenderThread = new VideoThread(triangleRenderer);
    triangleRenderer->moveToThread(videoRenderThread);

    QObject::connect(this,&MainWindow::setRenderFPS,triangleRenderer,&OpenGLRenderSurface::setFrameRate);

    QObject::connect(videoRenderThread,&QThread::started,triangleRenderer,&OpenGLRenderSurface::start);
    QObject::connect(videoRenderThread,&QThread::finished,this,[=]()
    {
        delete triangleRenderer;
        triangleRenderer = nullptr;
    });

    //Displays
    QObject::connect(triangleRenderer,&OpenGLRenderSurface::frameReady,videoRenderThread,&VideoThread::setFrame);

    //qDebug()<<QString("GUI thread:")<<QThread::currentThreadId();

    return true;
}

bool MainWindow::initializeUI()
{
    //UI setup
    QSize listSize(250,100);
    QSize buttonSize(250,25);
    QSize labelSize(250,25);

    OpenGLRenderer::OpenGLRenderSpecs previewSpecs = videoSpecs;
    previewSpecs.frameType.width = listSize.width();
    previewSpecs.frameType.height = listSize.width();

    previewDisplay = new OpenGLDisplay(mainUIScreen,
                                       previewSpecs,
                                       QSurfaceFormat::defaultFormat(),
                                       invertFilter->getOpenGLContext());
    previewDisplay->resize(previewSpecs.frameType.width,previewSpecs.frameType.height);

    QObject::connect(invertFilter,&InvertFilter::frameReady,previewDisplay,&OpenGLDisplay::setFrame);
    QWidget* previewDisplayContainer = QWidget::createWindowContainer(previewDisplay,
                                                                      mainWidget);
    previewDisplayContainer->setFixedSize(QSize(previewSpecs.frameType.width,previewSpecs.frameType.height));
    previewDisplayContainer->setSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed);

    textFPS = new QLineEdit(mainWidget);
    textFPS->setMinimumSize(labelSize);
    textFPS->setInputMask(QString("999"));//Allow numbers as input
    textFPS->setText(QString::number(videoSpecs.frameRate));

    lActualRenderFPS = new QLabel(mainWidget);
    lActualRenderFPS->setMinimumSize(labelSize);
    lActualRenderFPS->setText(QString("Actual fps:"));

    pbSetRenderFPS = new QPushButton(mainWidget);
    pbSetRenderFPS->setMinimumSize(buttonSize);
    pbSetRenderFPS->setText("Set fps");

    pbStartVideo = new QPushButton(mainWidget);
    pbStartVideo->setMinimumSize(buttonSize);
    pbStartVideo->setText("Start");

    pbStopVideo = new QPushButton(mainWidget);
    pbStopVideo->setMinimumSize(buttonSize);
    pbStopVideo->setText("Stop");

    mainLayout->setSpacing(/*previewDisplayContainer->width()/16.0*/0);
    mainLayout->setMargin(previewDisplayContainer->width()/16.0);

    mainLayout->addWidget(previewDisplayContainer,-1,Qt::AlignCenter);

    mainLayout->addWidget(textFPS,-1,Qt::AlignCenter);
    mainLayout->addWidget(lActualRenderFPS,-1,Qt::AlignCenter);
    mainLayout->addWidget(pbSetRenderFPS,-1,Qt::AlignCenter);

    mainLayout->addWidget(pbStartVideo,-1,Qt::AlignCenter);
    mainLayout->addWidget(pbStopVideo,-1,Qt::AlignCenter);

    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    //UI connections
    QObject::connect(pbStartVideo,&QPushButton::released,triangleRenderer,&OpenGLRenderSurface::start);
    QObject::connect(pbStopVideo,&QPushButton::released,triangleRenderer,&OpenGLRenderSurface::stop);

    QObject::connect(pbSetRenderFPS,&QPushButton::released,this,[=]()
    {
        emit setRenderFPS(textFPS->text().toDouble());
    });

    QObject::connect(triangleRenderer,&OpenGLRenderSurface::renderedFrame,this,[=](double actualFPS)
    {
        t_endRender = std::chrono::high_resolution_clock::now();
        t_deltaRender = t_endRender - t_startRender;

        if(t_deltaRender.count() >= textRefreshTime)
        {
            t_startRender = t_endRender;
            lActualRenderFPS->setText(QString("Actual fps: %1").arg(static_cast<unsigned int>(actualFPS)));
        }
    });

    return true;
}
