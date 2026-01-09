//
// Created by clan4 on 2026/1/7.
//

#ifndef ICONFONTVIEWER_MAINWINDOW_H
#define ICONFONTVIEWER_MAINWINDOW_H


#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include "TtfFontItem.h"
#include "ttfparser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void onLoadFont();
    void showFont(const QString& filePath);
    void updateSelector() const;
    void onSaveSvg();
    void onSearch(const QString &text);

private:
    QLabel* _lbLogo{};
    QLabel* _lbTitle{};
    QPushButton* _btnTheme{};
    QPushButton* _btnLoad{};
    QPushButton* _btnSave{};
    QLineEdit* _leSearch{};
    TtfFontList* _list{};

    QLabel* _lbSelector{};
    QLabel* _lbStatus{};

    cstk_ttf::TtfParser _ttfParser;

    QString _fontPath{};
};


#endif //ICONFONTVIEWER_MAINWINDOW_H
