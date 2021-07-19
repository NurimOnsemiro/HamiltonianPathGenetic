#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    void initWeight();
    void initCityPos();
    void initGen();
    void goodness();

    void ranking();
    void crossGen();
    void mutation();

    void insertHeap(int item);
    int deleteHeap();
    ~Widget();
    
private:
    Ui::Widget *ui;

    QVector<QPoint> cityPos;
    QVector<int> cityWeight;
    QVector<int> gen;
    QVector<int> score;
    QVector<int> newGen;
    QVector<int> parent;
    QVector<int> rank;
    int numPlaces; //장소의 수
    bool isFinish; //완료 여부
    QVector<int> bestGen;
    int generation;
    int distance;
    const int MAX_HEAP_SIZE = 100;
    const int MAX_WEIGHT = 100000;
    int timerIntervalMs = 500;

    int heapSize;
    QVector<int> heap;
    QVector<int> numHeap;
    QVector<int> roulette;

    void paintEvent(QPaintEvent *);

public Q_SLOTS:
    void generate();
};

#endif // WIDGET_H
