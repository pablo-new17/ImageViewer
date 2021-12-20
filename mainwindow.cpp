#include <QDebug>
#include <QMessageBox>


#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_Config = new QSettings("Config.ini", QSettings::IniFormat);

    Restore_UI();





    m_Disk_Folder_Model = new QFileSystemModel();
    m_Disk_Folder_Model->setRootPath("");
    m_Disk_Folder_Model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    ui->treeView_Folder->setModel(m_Disk_Folder_Model);
    for (int i = 1; i <= 3; ++i)
    {
        ui->treeView_Folder->hideColumn(i);
    }

    m_Config->beginGroup("Application");
        QString Last_Folder = m_Config->value("Folder", QDir::currentPath()).toString();
    m_Config->endGroup();

    ui->treeView_Folder->setCurrentIndex(m_Disk_Folder_Model->index(Last_Folder));
    ui->treeView_Folder->scrollTo(m_Disk_Folder_Model->index(Last_Folder));
    on_treeView_Folder_pressed(m_Disk_Folder_Model->index(Last_Folder));

    ui->treeView_Folder->show();

    ui->listWidget_Pictures->setViewMode(QListWidget::IconMode);
    ui->listWidget_Pictures->setIconSize(QSize(64, 64));
    ui->listWidget_Pictures->setResizeMode(QListWidget::Fixed);

    ui->label_Preview->setScaledContents( true );
    ui->label_Preview->setAlignment(Qt::AlignCenter);
    ui->label_Preview->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );


    // Create ZExifToolProcess
    etProcess= new ZExifToolProcess(this);
  #if defined Q_OS_LINUX || defined Q_OS_MACOS
    etProcess->setProgram(QLatin1String("./exiftool"));
  #elif defined Q_OS_WINDOWS
    etProcess->setProgram(QLatin1String("./exiftool.exe"));
  #endif
    connect(etProcess, &ZExifToolProcess::cmdCompleted,  this, &MainWindow::onEtCmdCompleted);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Restore_UI()
{
    m_Config->beginGroup("UI");
        //Main Windows
        int Window_Width = m_Config->value("MainWindow_W", 800).toInt();
        int Window_Height = m_Config->value("MainWindow_h", 600).toInt();
        this->resize(Window_Width, Window_Height);

        //Left U/D Split
        int Split_Left = m_Config->value("Split_L", 400).toInt();
        ui->splitter_Left->setSizes(QList<int>() << Split_Left << this->height() - Split_Left);

        //Middle L/R Split
        int Split_M1 = m_Config->value("Split_M1", 250).toInt();
        int Split_M2 = m_Config->value("Split_M2", 250).toInt();
        int Split_M3 = this->width() - Split_M2;
        ui->splitter_Middle->setSizes(QList<int>() << Split_M1 << Split_M2 << Split_M3);

        //Right U/D Split
        int Split_Right = m_Config->value("Split_R", 400).toInt();
        ui->splitter_Right->setSizes(QList<int>() << Split_Right << this->height() - Split_Right);

    m_Config->endGroup();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    m_Config->beginGroup("UI");
        m_Config->setValue("MainWindow_W", this->width());
        m_Config->setValue("MainWindow_h", this->height());
    m_Config->endGroup();
    m_Config->sync();
}

void MainWindow::on_splitter_Middle_splitterMoved(int pos, int index)
{
    m_Config->beginGroup("UI");

    if(index==1)
    {
        m_Config->setValue("Split_M1", pos);
    }
    else
    {
        m_Config->setValue("Split_M2", pos);
    }
    m_Config->endGroup();
    m_Config->sync();
}

void MainWindow::on_splitter_Left_splitterMoved(int pos, int index)
{
    m_Config->beginGroup("UI");
        m_Config->setValue("Split_L", pos);
    m_Config->endGroup();
    m_Config->sync();
}

void MainWindow::on_splitter_Right_splitterMoved(int pos, int index)
{
    m_Config->beginGroup("UI");
        m_Config->setValue("Split_R", pos);
    m_Config->endGroup();
    m_Config->sync();
}



void MainWindow::on_treeView_Folder_pressed(const QModelIndex &index)
{
    QString Last_Folder = m_Disk_Folder_Model->filePath(index);

    m_Config->beginGroup("Application");
        m_Config->setValue("Folder", Last_Folder);
    m_Config->endGroup();
    m_Config->sync();

    ui->listWidget_Pictures->clear();

    QDir directory(m_Disk_Folder_Model->filePath(index));
//    Print all writable file extensions (-listWF)
//    360 3G2 3GP 3GP2 3GPP AAX AI AIT APNG ARQ ARW AVIF CIFF CR2 CR3 CRM CRW CS1
//    DCP DNG DR4 DVB EPS EPS2 EPS3 EPSF ERF EXIF EXV F4A F4B F4P F4V FFF FLIF GIF
//    GPR HDP HEIC HEIF HIF ICC ICM IIQ IND INDD INDT INSP J2K JNG JP2 JPE JPEG JPF
//    JPG JPM JPS JPX JXL JXR LRV M4A M4B M4P M4V MEF MIE MNG MOS MOV MP4 MPO MQV
//    MRW NEF NRW ORF ORI PBM PDF PEF PGM PNG PPM PS PS2 PS3 PSB PSD PSDT QT RAF
//    RAW RW2 RWL SR2 SRW THM TIF TIFF VRD WDP X3F XMP

    directory.setNameFilters({"*.jpg", "*.JPG", "*.png", "*.PNG", "*.gif", "*.GIF"});
    for(const QFileInfo& info: directory.entryInfoList())
    {
        QListWidgetItem *item = new QListWidgetItem(QIcon(info.absoluteFilePath()), info.fileName());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
        ui->listWidget_Pictures->addItem(item);
    }
}

void MainWindow::on_listWidget_Pictures_itemClicked(QListWidgetItem *item)
{
    QString Image_File = m_Disk_Folder_Model->filePath(ui->treeView_Folder->currentIndex()) + "/" + item->text();

    QPixmap pixmap(Image_File);
    ui->label_Preview->setPixmap(pixmap.scaled(ui->label_Preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Start ZExifToolProcess
    etProcess->start();
    if(!etProcess->waitForStarted(500)) {
        etProcess->kill();
        return;
    }

    QByteArrayList cmdArgs;
    cmdArgs << "-XPComment";
    cmdArgs << Image_File.toLocal8Bit();

    // Send command to ZExifToolProcess
    qDebug() << cmdArgs;
    etProcess->command(cmdArgs);

    bool Enabled = false;
    for(int Index = 0; Index < ui->listWidget_Pictures->count(); Index++)
    {
        if( ui->listWidget_Pictures->item(Index)->checkState() == Qt::Checked)
            Enabled = true;
    }

    ui->pushButton_Edit->setEnabled(Enabled);
}

void MainWindow::onEtCmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel)
{
    Q_UNUSED(cmdId)
    Q_UNUSED(execTime)

    QString Message = QString(cmdOutputChannel);

    if(Message.isEmpty())
         Message = QString(cmdErrorChannel);
    else
        Message = Message.section(":", 1, 1).trimmed();

    ui->textEdit_Info->setPlainText(Message);

    // Stop ZExifToolProcess on command complete
    etProcess->terminate();
}





void MainWindow::on_pushButton_Edit_clicked()
{
    // Start ZExifToolProcess
    etProcess->start();
    if(!etProcess->waitForStarted(500)) {
        etProcess->kill();
        return;
    }

    QByteArrayList cmdArgs;
    QString Comment = QString("-XPComment=%1").arg(ui->textEdit_Info->toPlainText());
    bool Write = false;

    cmdArgs << Comment.toLocal8Bit();

    for(int Index = 0; Index < ui->listWidget_Pictures->count(); Index++)
    {
        QListWidgetItem* item = ui->listWidget_Pictures->item(Index);
        if(item->checkState())
        {
            QString Image_File = m_Disk_Folder_Model->filePath(ui->treeView_Folder->currentIndex()) + "/" + item->text();
            cmdArgs << Image_File.toLocal8Bit();
            Write = true;
        }
    }

    if(Write)
    {
        // Send command to ZExifToolProcess
        qDebug() << cmdArgs;
        etProcess->command(cmdArgs);
    }
}


void MainWindow::on_action_triggered()
{
    QMessageBox::about(this, "關於...", "版本: 0.1.0.20211218");
}

