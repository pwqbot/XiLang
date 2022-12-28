#pragma once

#include "ui_lpf.h"

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QTextStream>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>

namespace Ui
{
class lpfClass;
}

class lpf : public QMainWindow
{
    Q_OBJECT

  public:
    lpf(QWidget *parent = nullptr);
    ~lpf();
  private slots:
    void on_runButton_clicked();
    void on_openButton_clicked();

  private:
    Ui::lpfClass *ui;

    QString FilePath;
    QMenu  *menu;    

    void ShowftableToWindow(std::string);   
    void ShowResultToWindow(std::string);   
    void ShowPcodeToWindow(std::string);    
    void ShowAstToWindow(std::string); 
};
