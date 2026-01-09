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
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QFont>
#include <QFileInfo>
#include <QPixmap>
#include <QStyleHints>
#include <QStatusBar>
#include <QColorDialog>
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
    _btnSave = new QPushButton(this);
    setupBtn(_btnLoad, "Load", 0xE8E5, "Load TTF Font");
    setupBtn(_btnTheme, "Theme", 0xE793, "Change Application Theme");
    setupBtn(_btnSave, "Save", 0xE74E, "Save SVG Image");

    _leSearch = new QLineEdit(this);
    _leSearch->setPlaceholderText("Search Icon");
    _leSearch->setFixedSize(250, _lbTitle->minimumSizeHint().height());

    _list = new TtfFontList();
    _list->setMinimumHeight(300);

    auto loTop = new QHBoxLayout;
    loTop->setSpacing(10);
    loTop->setContentsMargins(10, 10, 10, 10);
    loTop->addWidget(_lbLogo);
    loTop->addWidget(_lbTitle);
    loTop->addStretch();
    loTop->addWidget(_leSearch);
    loTop->addWidget(_btnLoad);
    loTop->addWidget(_btnSave);
    loTop->addWidget(_btnTheme);

    auto loMain = new QVBoxLayout;
    loMain->addLayout(loTop);
    loMain->addWidget(_list, 1);

    _lbSelector = new QLabel("0 / 0 / 0", this);
    _lbSelector->setContentsMargins(10, 0, 10, 0);
    _lbSelector->setAlignment(Qt::AlignCenter);
    _lbSelector->setFixedWidth(150);

    _lbStatus = new QLabel(this);
    _lbStatus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _lbStatus->setContentsMargins(10, 0, 10, 0);

    auto* statusBar = new QStatusBar(this);
    statusBar->addPermanentWidget(_lbSelector);
    statusBar->addPermanentWidget(_lbStatus, 1);

    statusBar->setSizeGripEnabled(false);
    setStatusBar(statusBar);

    central->setLayout(loMain);

    // setMinimumSize(500, 500);
    resize(1000, 1000);

    connect(_btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFont);
    connect(_list, &TtfFontList::loadFile, this, &MainWindow::showFont);
    connect(_list, &TtfFontList::itemSelectionChanged, this, &MainWindow::updateSelector);
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
    connect(_btnSave, &QPushButton::clicked, this, &MainWindow::onSaveSvg);
    connect(_leSearch, &QLineEdit::textChanged, this, &MainWindow::onSearch);
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
    _ttfParser = cstk_ttf::TtfParser(filePath);
    if (!_ttfParser.isValid())
    {
        QMessageBox::critical(this, "Error", _ttfParser.getErrorMessage());
        _lbStatus->setText(_ttfParser.getErrorMessage());
        return;
    }

    _fontPath = filePath;
    const auto ttfFont = _ttfParser.getFont();
    const auto ttfData = _ttfParser.getDataList();
    setWindowTitle(QString("IconfontViewer - %1").arg(filePath));
    _lbStatus->setText(QString("Load TTF Success: %1").arg(ttfFont.family()));
    _list->clear();
    for (const auto& data : ttfData)
    {
        auto widget = new TtfFontItem(ttfFont, data.unicode, data.name);
        auto item = new QListWidgetItem();
        item->setSizeHint(widget->sizeHint());
        _list->addItem(item);
        _list->setItemWidget(item, widget);
    }
    updateSelector();
}

void MainWindow::updateSelector() const
{
    int showItems = 0;

    for (int i = 0; i < _list->count(); ++i)
    {
        auto item = _list->item(i);
        auto widget = qobject_cast<TtfFontItem*>(_list->itemWidget(item));
        if (widget)
        {
            if (!item->isHidden())
            {
                showItems++;
            }
            else
            {
                item->setSelected(false);
            }
            const bool isSelected = item->isSelected();
            widget->setProperty("selected", isSelected);
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
        }
    }
    _lbSelector->
        setText(QString("%1 / %2 / %3").arg(_list->selectedItems().count()).arg(showItems).arg(_list->count()));
}

void MainWindow::onSaveSvg()
{
    if (_list->selectedItems().count() <= 0)
    {
        QMessageBox::critical(this, "Error", "No icon selected");
        _lbStatus->setText("No icon selected");
        return;
    }
    QString filePath = QFileDialog::getExistingDirectory(this, "Save SVG Image to Folder", "");
    if (filePath.isEmpty())
    {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "SVG Image Color");
    if (!color.isValid())
    {
        return;
    }

    auto fileName = QFileInfo(_fontPath).baseName();
    int saveCount = 0;
    for (const auto& item : _list->selectedItems())
    {
        auto widget = qobject_cast<TtfFontItem*>(_list->itemWidget(item));
        if (widget)
        {
            QString svgName = QString("%1_U+%2_%3.svg").arg(
                fileName,
                QString("%1").arg(widget->getUnicode(), 4, 16, QLatin1Char('0')).toUpper(),
                widget->getName()
            );

            for (QChar& ch : svgName)
            {
                if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' || ch == '?' ||
                    ch == '"' || ch == '<' || ch == '>' || ch == '|')
                {
                    svgName.replace(ch, '_');
                }
            }

            QFile saveFile(filePath + "/" + svgName);
            if (!saveFile.open(QIODeviceBase::WriteOnly))
            {
                QMessageBox::critical(this, "Error", "Failed to save SVG");
                _lbStatus->setText("Failed to save SVG");
                return;
            }
            QTextStream saveStream(&saveFile);
            saveStream << _ttfParser.getSvgData(widget->getUnicode(), color.name());
            saveStream.flush();
            saveFile.close();
            saveCount++;
            _lbStatus->setText(QString("%1 - %2 : %3").arg(saveCount).arg(svgName, _ttfParser.getErrorMessage()));
        }
    }
    _lbStatus->setText(QString("Save %1 svg Files Success").arg(saveCount));
}

void MainWindow::onSearch(const QString& text)
{
    for (int i = 0; i < _list->count(); ++i)
    {
        auto item = _list->item(i);
        auto widget = qobject_cast<TtfFontItem*>(_list->itemWidget(item));
        if (widget)
        {
            if (widget->getUnicodeString().contains(text, Qt::CaseInsensitive) || widget->getName().contains(
                text, Qt::CaseInsensitive))
            {
                item->setHidden(false);
            }
            else
            {
                item->setHidden(true);
            }
        }
    }
    updateSelector();
}
