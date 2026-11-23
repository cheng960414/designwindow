#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QKeySequence>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Image Processor v1.0"));
    resize(900, 650);

    createActions();
    createMenus();
    createToolBars();
    createCentralWorkspace();

    statusBar()->showMessage(tr("就緒 - 請開啟影像檔案"));
}

void MainWindow::createActions()
{
    // 1. Open Image (套用 icon 與 Ctrl+O 快捷鍵)
    m_openAction = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open Image"), this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip(tr("開啟影像檔案"));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenImage);

    // 2. Save (套用 icon 與 Ctrl+S 快捷鍵)
    m_saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), this);
    m_saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    m_saveAction->setStatusTip(tr("儲存目前影像"));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSave);

    // 3. Exit (套用 icon 與 Ctrl+Q 快捷鍵)
    m_exitAction = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Exit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("離開應用程式"));
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // 4. Edit 選單動作：Reset 還原原圖 (套用 icon 與 Ctrl+Z 快捷鍵)
    m_resetAction = new QAction(style()->standardIcon(QStyle::SP_DialogResetButton), tr("Reset Image"), this);
    m_resetAction->setShortcut(QKeySequence(tr("Ctrl+Z")));
    m_resetAction->setStatusTip(tr("還原至原始載入影像"));
    connect(m_resetAction, &QAction::triggered, this, &MainWindow::onReset);

    // 5. Image 選單動作：Grayscale 灰階轉換 (套用 icon 與 Ctrl+G 快捷鍵)
    m_grayscaleAction = new QAction(style()->standardIcon(QStyle::SP_DesktopIcon), tr("Grayscale"), this);
    m_grayscaleAction->setShortcut(QKeySequence(tr("Ctrl+G")));
    m_grayscaleAction->setStatusTip(tr("轉換為灰階影像"));
    connect(m_grayscaleAction, &QAction::triggered, this, &MainWindow::onGrayscale);
}

void MainWindow::createMenus()
{
    // File Menu
    m_fileMenu = menuBar()->addMenu(tr("File"));
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    // Edit Menu
    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addAction(m_resetAction);

    // Image Menu
    m_imageMenu = menuBar()->addMenu(tr("Image"));
    m_imageMenu->addAction(m_grayscaleAction);
}

void MainWindow::createToolBars()
{
    // Toolbar 加入常見 QAction，與 File Menu 共用同一個 m_openAction 執行相同功能
    m_mainToolBar = addToolBar(tr("Main ToolBar"));
    m_mainToolBar->setMovable(false);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_resetAction);
    m_mainToolBar->addAction(m_grayscaleAction);
}

void MainWindow::createCentralWorkspace()
{
    m_centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_centralWidget);
    layout->setContentsMargins(10, 10, 10, 10);

    m_imageLabel = new QLabel(tr("CentralWidget 影像工作區（請點擊 Open Image 或按 Ctrl+O 載入圖片）"), m_centralWidget);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("QLabel { background-color: #2b2b2b; color: #cccccc; border: 1px dashed #666666; }");
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    layout->addWidget(m_imageLabel);
    setCentralWidget(m_centralWidget);
}

void MainWindow::onOpenImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp)")
        );

    if (filePath.isEmpty()) {
        return;
    }

    QImage loadedImage;
    if (!loadedImage.load(filePath)) {
        QMessageBox::warning(this, tr("開啟失敗"), tr("無法載入指定的影像檔案。"));
        return;
    }

    m_originalImage = loadedImage;
    m_currentImage = loadedImage;
    updateImageDisplay();
    statusBar()->showMessage(tr("已開啟：%1 (%2 x %3)")
                                 .arg(filePath)
                                 .arg(m_currentImage.width())
                                 .arg(m_currentImage.height()));
}

void MainWindow::onSave()
{
    if (m_currentImage.isNull()) {
        QMessageBox::information(this, tr("提示"), tr("目前沒有可儲存的影像，請先開啟圖片。"));
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        tr("output.png"),
        tr("PNG Image (*.png);;JPEG Image (*.jpg);;BMP Image (*.bmp)")
        );

    if (savePath.isEmpty()) {
        return;
    }

    if (m_currentImage.save(savePath)) {
        statusBar()->showMessage(tr("影像已儲存至：%1").arg(savePath));
    } else {
        QMessageBox::warning(this, tr("儲存失敗"), tr("無法寫入檔案。"));
    }
}

void MainWindow::onReset()
{
    if (m_originalImage.isNull()) {
        return;
    }

    m_currentImage = m_originalImage;
    updateImageDisplay();
    statusBar()->showMessage(tr("已還原至原始影像"));
}

void MainWindow::onGrayscale()
{
    if (m_currentImage.isNull()) {
        statusBar()->showMessage(tr("請先載入影像再執行濾鏡"));
        return;
    }

    m_currentImage = m_currentImage.convertToFormat(QImage::Format_Grayscale8);
    updateImageDisplay();
    statusBar()->showMessage(tr("已套用灰階效果"));
}

void MainWindow::updateImageDisplay()
{
    if (m_currentImage.isNull()) {
        return;
    }

    QPixmap pixmap = QPixmap::fromImage(m_currentImage);
    m_imageLabel->setPixmap(pixmap.scaled(
        m_imageLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateImageDisplay();
}