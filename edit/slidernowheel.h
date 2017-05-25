#ifndef SLIDERNOWHEEL_H
#define SLIDERNOWHEEL_H

#include <QSlider>
#include <QWheelEvent>

class SliderNoWheel : public QSlider
{
    Q_OBJECT

public:
    SliderNoWheel(QWidget * parent) : QSlider(parent)
    {}

    ~SliderNoWheel()
    {}

protected:
    virtual void wheelEvent(QWheelEvent *event) {
        event->ignore();
    }
};

#endif // SLIDERNOWHEEL_H
