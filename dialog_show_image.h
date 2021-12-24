#ifndef DIALOG_SHOW_IMAGE_H
#define DIALOG_SHOW_IMAGE_H

#include <QDialog>

namespace Ui {
class Dialog_Show_Image;
}

class Dialog_Show_Image : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Show_Image( QWidget *parent = nullptr);
    ~Dialog_Show_Image();

    const QString &Image_File() const;
    void setImage_File(const QString &newImage_File);

private slots:
    void on_checkBox_toggled(bool checked);

private:
    Ui::Dialog_Show_Image*  ui;
    QString                 m_Image_File;
};

#endif // DIALOG_SHOW_IMAGE_H
