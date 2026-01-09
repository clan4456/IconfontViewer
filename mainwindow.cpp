//
// Created by clan4 on 2026/1/7.
//

#include "mainwindow.h"
#include "ttfparser.h"
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QDir>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QFont>
#include <QPixmap>
#include <QStyleHints>
#include "TtfFontItem.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    _lbTitle = new QLabel(QApplication::applicationName(), this);
    _lbTitle->setAlignment(Qt::AlignLeft);
    QFont titleFont = _lbTitle->font();
    titleFont.setPointSize(24);
    _lbTitle->setFont(titleFont);

    _lbLogo = new QLabel(this);
    auto logo = QPixmap(":res/res/logo.png");
    const auto logoHeight = static_cast<double>(_lbTitle->sizeHint().height()) * 0.8;
    logo = logo.scaledToHeight(static_cast<int>(logoHeight), Qt::SmoothTransformation);
    _lbLogo->setPixmap(logo);

    auto setupBtn = [this](QPushButton* btn, const QString& name, const quint32 iconUnicode, const QString& toolTip)
    {
        constexpr const char* iconFontName = "Segoe MDL2 Assets";
        btn->setToolTip(toolTip);
        if (QFontDatabase::hasFamily(iconFontName))
        {
            btn->setFixedSize(_lbTitle->minimumSizeHint().height(), _lbTitle->minimumSizeHint().height());
            auto font = QFont(iconFontName);
            font.setPointSize(18);
            btn->setFont(font);
            btn->setText(QChar(iconUnicode));
        }
        else
        {
            btn->setFixedSize(_lbTitle->minimumSizeHint().height() * 2, _lbTitle->minimumSizeHint().height());
            btn->setText(name);
        }
    };

    _btnLoad = new QPushButton(this);
    _btnTheme = new QPushButton(this);
    setupBtn(_btnLoad, "Load", 0xE8E5, "Load TTF Font");
    setupBtn(_btnTheme, "Theme", 0xE793, "Change Application Theme");

    _list = new TtfFontList();

    QHBoxLayout* loTop = new QHBoxLayout;
    loTop->setSpacing(10);
    loTop->setContentsMargins(10, 10, 10, 10);
    loTop->addWidget(_lbLogo);
    loTop->addWidget(_lbTitle);
    loTop->addStretch();
    loTop->addWidget(_btnLoad);
    loTop->addWidget(_btnTheme);

    QVBoxLayout* loMain = new QVBoxLayout;
    loMain->addLayout(loTop);
    loMain->addWidget(_list, 1);
    central->setLayout(loMain);

    setMinimumSize(500, 500);
    resize(1000, 1000);

    connect(_btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFont);
    connect(_list, &TtfFontList::loadFile, this, &MainWindow::showFont);
    connect(_btnTheme, &QPushButton::clicked, this, [this]()
    {
        if (QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
        {
            QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
        }
        else
        {
            QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
        }
        if (!_fontPath.isEmpty())
        {
            showFont(_fontPath);
        }
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::onLoadFont()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open TTF Font", "", "TTF Files (*.ttf);;All Files (*)");
    if (!filePath.isEmpty())
    {
        showFont(filePath);
    }
}

void MainWindow::showFont(const QString& filePath)
{
    auto ttf = cstk_ttf::TtfParser(filePath);
    if (!ttf.isValid())
    {
        QMessageBox::critical(this, "Error", ttf.getErrorMessage());
        return;
    }

    _fontPath = filePath;
    const auto ttfFont = ttf.getFont();
    const auto ttfData = ttf.getData();
    setWindowTitle(QString("IconfontViewer [%1] %2").arg(ttfData.count()).arg(filePath));
    _list->clear();

    for (const auto& key : ttfData.keys())
    {
        auto widget = new TtfFontItem(ttfFont, key, ttfData.value(key));
        auto item = new QListWidgetItem();
        item->setSizeHint(widget->sizeHint());
        _list->addItem(item);
        _list->setItemWidget(item, widget);
    }
}
