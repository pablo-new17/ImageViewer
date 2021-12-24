#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileSystemModel>
#include <QListWidgetItem>

#include "ZExifTool/ZExifToolProcess.h"
#include "dialog_show_image.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void Restore_UI();

    void on_splitter_Left_splitterMoved(int pos, int index);

    void on_splitter_Middle_splitterMoved(int pos, int index);

    void on_splitter_Right_splitterMoved(int pos, int index);

    void on_treeView_Folder_pressed(const QModelIndex &index);

    void on_listWidget_Pictures_itemClicked(QListWidgetItem *item);

    void onEtCmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel);
//    void onEtCmdCompleted();



    void on_pushButton_Edit_clicked();

    void on_action_triggered();

    void on_listWidget_Pictures_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow*     ui;
    QSettings*          m_Config;
    QFileSystemModel*   m_Disk_Folder_Model;
    ZExifToolProcess*   etProcess;
    Dialog_Show_Image   m_Image_View;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H
