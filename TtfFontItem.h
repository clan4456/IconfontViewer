#ifndef ICONFONTVIEWER_TTFFONTITEM_H
#define ICONFONTVIEWER_TTFFONTITEM_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMouseEvent>

class TtfFontList : public QListWidget
{
    Q_OBJECT

public:
    explicit TtfFontList(QWidget* parent = nullptr);
    ~TtfFontList() override;

signals:
    void loadFile(const QString& file);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent* event) override;
};

class TtfFontItem : public QFrame
{
    Q_OBJECT

public:
    explicit TtfFontItem(QFont font, const quint32 unicode, const QString& name, QWidget* parent = nullptr);
    ~TtfFontItem() override;

    QSize sizeHint() const override;

private:
    QLabel* _lbIcon;
    QLabel* _lbName;
    QLabel* _lbUnicode;

    quint32 _unicode;
    QString _name;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif //ICONFONTVIEWER_TTFFONTITEM_H
