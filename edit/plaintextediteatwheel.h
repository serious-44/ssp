#ifndef PLAINTEXTEDITEATWHEEL_H
#define PLAINTEXTEDITEATWHEEL_H


#include <QPlainTextEdit>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>

class PlainTexteditEatWheel : public QPlainTextEdit
{
    Q_OBJECT

public:
    PlainTexteditEatWheel(QWidget * parent) : QPlainTextEdit(parent)
    {}

    ~PlainTexteditEatWheel()
    {}

protected:
    virtual void wheelEvent(QWheelEvent *event) {
        //QWheelEvent e(event->pos(), event->globalPos(), event->pixelDelta(), event->angleDelta(), event->angleDelta().y(), Qt::Vertical, event->buttons(), event->modifiers());
        //qDebug() << "<" << event->angleDelta().y() << verticalScrollBar()->minimum() << verticalScrollBar()->maximum() << verticalScrollBar()->value();
        if (event->angleDelta().y() < 0 ? verticalScrollBar()->value() < verticalScrollBar()->maximum() : verticalScrollBar()->value() > verticalScrollBar()->minimum()) {
            QPlainTextEdit::wheelEvent(event);
        }
        //qDebug() << ">" << event->angleDelta().y() << verticalScrollBar()->minimum() << verticalScrollBar()->maximum() << verticalScrollBar()->value();

        event->accept();
    }

    virtual void leaveEvent(QEvent *event) {
        if (hasFocus()) {
            ((QWidget*)(parent()->parent()->parent()))->setFocus();
        }
    }
    virtual void enterEvent(QEvent *event) {
        setFocus();
    }
    virtual void focusInEvent(QFocusEvent *event) {
        //qDebug() << "focus in";
        QPlainTextEdit::focusInEvent(event);
        setStyleSheet("background-color:#ffffff");
    }
    virtual void focusOutEvent(QFocusEvent *event) {
        //qDebug() << "focus out";
        QPlainTextEdit::focusOutEvent(event);
        setStyleSheet("background-color:#f2f2f2");
    }
};

#endif // PLAINTEXTEDITEATWHEEL_H
