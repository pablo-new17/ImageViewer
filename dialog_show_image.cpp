#include "dialog_show_image.h"
#include "ui_dialog_show_image.h"

Dialog_Show_Image::Dialog_Show_Image(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Show_Image)
{
    ui->setupUi(this);
}

Dialog_Show_Image::~Dialog_Show_Image()
{
    delete ui;
}

void Dialog_Show_Image::on_checkBox_toggled(bool checked)
{
    ui->label_Image->setScaledContents(checked);
}

const QString &Dialog_Show_Image::Image_File() const
{
    return m_Image_File;
}

void Dialog_Show_Image::setImage_File(const QString &newImage_File)
{
    m_Image_File = newImage_File;

    QPixmap pixmap(m_Image_File);
    ui->label_Image->setPixmap(pixmap);
}

