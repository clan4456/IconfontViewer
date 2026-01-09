#include "TtfFontItem.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QMimeData>
#include <QUrl>
#include <QClipboard>
#include <QMessageBox>
#include <QTimer>

TtfFontList::TtfFontList(QWidget* parent) : QListWidget(parent)
{
    setViewMode(QListView::IconMode);
    setSelectionMode(QAbstractItemView::NoSelection);
    setResizeMode(QListView::Adjust);
    setFlow(QListView::LeftToRight);
    setDragEnabled(false);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setSpacing(10);
}

TtfFontList::~TtfFontList()
{
}

void TtfFontList::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void TtfFontList::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TtfFontList::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList)
        {
            QString filePath = url.toLocalFile();
            emit loadFile(filePath);
        }
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

TtfFontItem::TtfFontItem(QFont font, const quint32 unicode, const QString& name, QWidget* parent) : QFrame(parent),
    _unicode(unicode),
    _name(name)
{
    _lbIcon = new QLabel(this);
    font.setPointSize(48);
    _lbIcon->setFont(font);
    _lbIcon->setText(QChar(unicode));
    _lbIcon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);


    _lbName = new QLabel(name, this);
    _lbName->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    _lbUnicode = new QLabel(QString("U+%1").arg(unicode, 4, 16, QLatin1Char('0')).toUpper(), this);
    _lbUnicode->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    auto lo = new QVBoxLayout(this);
    lo->addStretch();
    lo->addWidget(_lbIcon);
    lo->addWidget(_lbUnicode);
    lo->addWidget(_lbName);
    lo->addStretch();
    setStyleSheet(
        "TtfFontItem {border-style: solid; border-width: 2px; border-color: gray; border-radius: 10px;} TtfFontItem:hover {border-color: #BBDEFB;}");
}

TtfFontItem::~TtfFontItem()
{
}

QSize TtfFontItem::sizeHint() const
{
    return QSize(150, 150);
}

void TtfFontItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    const auto str = QString("%1").arg(_unicode, 4, 16, QLatin1Char('0')).toUpper();
    QApplication::clipboard()->setText(str);
    auto* msgBox = new QMessageBox(QMessageBox::Information, "Copied", QString("Copied: %1").arg(str));
    QTimer::singleShot(500, msgBox, [msgBox]() { msgBox->accept(); });
    msgBox->exec();
}
