#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QLabel>
#include <QImage>
#include <QPixmap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onOpenImage();
    void onSave();
    void onReset();
    void onGrayscale();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createCentralWorkspace();
    void updateImageDisplay();

    // QAction 動作
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_exitAction;
    QAction *m_resetAction;
    QAction *m_grayscaleAction;

    // File / Image / Edit 三大選單與工具列
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_imageMenu;
    QToolBar *m_mainToolBar;

    // 中央工作區與影像資料
    QWidget *m_centralWidget;
    QLabel *m_imageLabel;
    QImage m_originalImage;
    QImage m_currentImage;
};

#endif // MAINWINDOW_H