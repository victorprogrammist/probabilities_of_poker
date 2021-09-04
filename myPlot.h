
#ifndef __MY_PLOT_H__
#define __MY_PLOT_H__

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class MyPlot: public QWidget {
    QGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;

    void resizeEvent(QResizeEvent *) override { repaintPlot(); }
    void paintPixmap(const QPixmap& pixmap);
    void recalcRect();

    QPolygonF getListPoints(uint iList, int wi, int he);

    struct Poly {
        QPolygonF points;
        QColor colorLine;
        QColor colorPoints;
    };

    QList<Poly> m_listPoints;

    QRectF rectPoints;

public:
    MyPlot(QWidget* parent = nullptr);

    void appendPoints(const QVector<QPointF>& listPoints);

    void appendPoints(
            const QVector<QPointF>& listPoints,
            QColor color);

    void appendPoints(
            const QVector<QPointF>& listPoints,
            QColor colorLine,
            QColor colorPoints);

    void repaintPlot();
};

#endif

