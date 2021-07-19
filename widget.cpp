#include "widget.h"
#include "ui_widget.h"
#include <QTime>
#include <QPainter>
#include <QDebug>
#include <qmath.h>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(500,500);
    setWindowTitle("Genetic Algorithm");

    numPlaces=100;

    {
        QFile file;
        QString data;
        file.setFileName("./config.json");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
            auto obj = doc.object();
            numPlaces = obj["numberOfPlaces"].toInt();
            timerIntervalMs = obj["timerIntervalMs"].toInt();
        }
    }


    generation=0;
    distance=10000;

    qsrand(QTime::currentTime().msec());

    cityWeight.clear();
    cityPos.clear();
    gen.clear();
    heapSize=0;
    heap.clear();
    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        heap.append(0);
        numHeap.append(0);
    }


    isFinish=true;
    initCityPos();
    initWeight();
    initGen();

    QTimer* timer=new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(timerIntervalMs);
    connect(timer,SIGNAL(timeout()),this,SLOT(generate()));
    timer->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initWeight()
{
    for(int i=0;i<numPlaces;i++)
    {
        for(int j=0;j<numPlaces;j++)
        {
            if(j>i)
            {
                cityWeight.append((int)qSqrt((cityPos[i].x()-cityPos[j].x())*(cityPos[i].x()-cityPos[j].x())+(cityPos[i].y()-cityPos[j].y())*(cityPos[i].y()-cityPos[j].y())));
            }
            else
            {
                cityWeight.append(MAX_WEIGHT);
            }
        }
    }
    for(int i=0;i<numPlaces;i++)
    {
        for(int j=0;j<numPlaces;j++)
        {
            if(i>j)
            {
                cityWeight[i*numPlaces+j]=cityWeight[j*numPlaces+i];
            }
        }
    }
}

void Widget::ranking()
{
    bestGen.clear();
    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        heap[i]=0;
        numHeap[i]=0;
    }
    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        insertHeap(score[i]);
    }
    rank.clear();
    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        rank.append(deleteHeap());
    }

    distance=score[rank[99]];

    for(int i=0;i<numPlaces;i++)
    {
        bestGen.append(gen[rank[99]*numPlaces+i]);
    }
    parent.clear();
    for(int j=0;j<98;j++)
    {
        int select=(qrand()%5050)+1;
        int count=0;
        for(int i=1;i<=MAX_HEAP_SIZE;i++)
        {
            count+=i;
            if(select<=count)
            {
                parent.append(rank[i-1]);
                break;
            }
        }
    }
}

void Widget::crossGen()
{
    int pos1, pos2;
    do
    {
        pos1 = (qrand() * 3) % (numPlaces - 1);
        pos2 = ((qrand() * 7) % (numPlaces - 1)) + 1;

        if (pos1 > pos2)
        {
            int temp = pos1;
            pos1 = pos2;
            pos2 = temp;
        }
    } while (pos1 == pos2);

    newGen.clear();

    for(int i=0;i<98;i++)
    {
        for(int j=0;j<numPlaces;j++)
            newGen.append(gen[parent[i]*numPlaces+j]);
        for(int j=pos1;j<pos2;j++)
        {
            int temp1=qrand()%(pos2-pos1)+pos1;
            int temp2=qrand()%(pos2-pos1)+pos1;
            int temp=newGen[i*numPlaces+temp1];
            newGen[i*numPlaces+temp1]=newGen[i*numPlaces+temp2];
            newGen[i*numPlaces+temp2]=temp;
        }
    }

    for(int i=0;i<2;i++)
    {
        for(int j=0;j<numPlaces;j++)
        {
            newGen.append(gen[rank[99-i]*numPlaces+j]);
        }
    }

    gen.clear();
    gen=newGen;
}

void Widget::generate()
{
    if(isFinish==false)
        return;
    isFinish=false;
   // qDebug("1");
    goodness();
   // qDebug("2");
    ranking();
   // qDebug("3");
    crossGen();
    //qDebug("4");
    mutation();
    generation++;
    update();
    isFinish=true;
}

void Widget::mutation()
{
    int temp=qrand()%MAX_HEAP_SIZE;
    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        if(temp==qrand()%MAX_HEAP_SIZE)
        {
            for(int j=0;j<numPlaces;j++)
            {
                int pos1=qrand()%numPlaces;
                int pos2=(qrand()*3)%numPlaces;
                if(pos1==0 || pos2==0)
                    continue;
                int temp2=gen[i*numPlaces+pos1];
                gen[i*numPlaces+pos1]=gen[i*numPlaces+pos2];
                gen[i*numPlaces+pos2]=temp2;
            }
        }
    }
}

void Widget::initCityPos()
{
    for(int i=0;i<numPlaces;i++)
    {
        cityPos.append(QPoint(qrand()%490,qrand()%490));
        //qDebug()<<cityPos[i].x()<<","<<cityPos[i].y();
    }
}

void Widget::initGen()
{

    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        gen.append(0);
        QVector<int> tempVec;
        QVector<int> dieVec;
        tempVec.clear();
        dieVec.clear();
        for(int j=0;j<numPlaces - 1;j++)
            tempVec.append(j+1);
        int amount=numPlaces - 1;
        while(amount!=0)
        {
            int random=qrand()%amount;
            gen.append(tempVec[random]);
            tempVec[random]=tempVec[--amount];
        }
    }
    bestGen.clear();
    for(int i=0;i<numPlaces;i++)
    {
        bestGen.append(gen[i]);
    }
}

void Widget::goodness()
{
    score.clear();
    for(int i=0;i<MAX_HEAP_SIZE;i++)
        score.append(0);

    for(int i=0;i<MAX_HEAP_SIZE;i++)
    {
        int hap=0;
        for(int j=0;j<numPlaces;j++)
        {
            int dst;
            if(j!=numPlaces - 1)
                dst=gen[i*numPlaces+j+1];
            else
                dst=gen[i*numPlaces];
            hap+=cityWeight[gen[i*numPlaces+j]*numPlaces+dst];
        }
        score[i]=hap;
        //qDebug()<<"score"<<score[i];
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    painter.setPen(Qt::transparent);
    painter.drawRect(QRect(0,0,width(),height()));


    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    painter.drawRoundedRect(QRect(cityPos[0].x(),cityPos[0].y(),5,5),50,50);
    painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    for(int i=1;i<numPlaces;i++)
    {
        painter.drawRoundedRect(QRect(cityPos[i].x(),cityPos[i].y(),5,5),50,50);
    }
    painter.setPen(Qt::blue);

    for(int i=0;i<numPlaces;i++)
    {
        int dst;
        if(i!=numPlaces - 1)
            dst=bestGen[i+1];
        else
            dst=bestGen[0];
        if(cityWeight[bestGen[i]*numPlaces+dst]!=MAX_WEIGHT)
        {
            if(cityWeight[bestGen[i]*numPlaces+dst]<100)
            {
                painter.setPen(QColor(0,255,0));
            }
            else if(cityWeight[bestGen[i]*numPlaces+dst]<200)
            {
                painter.setPen(QColor(30,230,0));
            }
            else if(cityWeight[bestGen[i]*numPlaces+dst]<300)
            {
                painter.setPen(QColor(100,130,0));
            }
            else if(cityWeight[bestGen[i]*numPlaces+dst]<400)
            {
                painter.setPen(QColor(180,30,0));
            }
            else
            {
                painter.setPen(QColor(250,10,0));
            }
            painter.drawLine(cityPos[bestGen[i]],cityPos[dst]);
        }
    }

    painter.setPen(Qt::black);
    painter.drawText(QRect(0,0,500,30),QString("%1 generation, distance:%2").arg(generation).arg(distance));

}

void Widget::insertHeap(int item)
{
    int i;
    i = ++heapSize;

    while ((i != 1) && (item > heap[i / 2]))
    {
        heap[i] = heap[i / 2];
        numHeap[i]=numHeap[i/2];
        i /= 2;
    }
    heap[i] = item;
    numHeap[i]=heapSize-1;
}

int Widget::deleteHeap()
{
    int parent, child;
    int item, temp;


    item = numHeap[1];
    temp = heap[heapSize--];
    int temp2=numHeap[heapSize+1];
    parent = 1;
    child = 2;
    while (child <= heapSize)
    {
        if ((child < heapSize) && (heap[child]) < heap[child + 1])
            child++;
        if (temp >= heap[child])
            break;
        heap[parent] = heap[child];
        numHeap[parent] = numHeap[child];
        parent = child;
        child *= 2;
    }
    heap[parent] = temp;
    numHeap[parent]=temp2;

    return item;
}
