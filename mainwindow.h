//
// Created by clan4 on 2026/1/7.
//

#ifndef ICONFONTVIEWER_MAINWINDOW_H
#define ICONFONTVIEWER_MAINWINDOW_H


#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include "TtfFontItem.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onLoadFont();
    void showFont(const QString &filePath);

private:
    QLabel *_lbLogo{};
    QLabel *_lbTitle{};
    QPushButton *_btnTheme{};
    QPushButton *_btnLoad{};
    TtfFontList *_list{};

    QString _fontPath{};
};


#endif //ICONFONTVIEWER_MAINWINDOW_H