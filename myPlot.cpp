
#include "myPlot.h"
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QStyle>
#include <QDesktopServices>
#include <QScreen>

void MyPlot::appendPoints(const QVector<QPointF>& listPoints) {
    appendPoints(QPolygonF(listPoints), QColorConstants::Black, QColorConstants::Black);
}

void MyPlot::appendPoints(
        const QVector<QPointF>& listPoints,
        QColor color) {
    appendPoints(listPoints, color, color);
}

void MyPlot::appendPoints(
        const QVector<QPointF>& listPoints,
        QColor colorLine,
        QColor colorPoints
        ) {

    if (listPoints.size()) {
        m_listPoints.append({QPolygonF(listPoints), colorLine, colorPoints});
        recalcRect();
    }

    QTimer::singleShot(1, [this]{ repaintPlot(); });
}

void MyPlot::recalcRect() {

    if (m_listPoints.empty()) {
        rectPoints = QRectF();
        return;
    }

    rectPoints = m_listPoints.first().points.boundingRect();

    for (uint ii = 1; ii < m_listPoints.size(); ++ii)
        rectPoints = rectPoints.united(m_listPoints[ii].points.boundingRect());
}

QPolygonF MyPlot::getListPoints(uint iList, int wi, int he) {

    QPolygonF points;

    for (const QPointF& p: m_listPoints[iList].points) {

        double x = wi / 2.;
        double y = he / 2.;

        if (rectPoints.width())
            x = (p.x() - rectPoints.x()) / rectPoints.width() * (wi - 20) + 10;

        if (rectPoints.height())
            y = (p.y() - rectPoints.y()) / rectPoints.height() * (he - 20) + 10;

        y = he - y;

        points.append({x,y});
    }

    return points;
}

void MyPlot::repaintPlot() {
    if (!m_view || !m_scene) return;

    QRect r = m_view->contentsRect();
    m_view->setSceneRect(0, 0, r.width(), r.height());

    QSize sz = m_view->viewport()->size();
    int wi = sz.width();
    int he = sz.height();

    QPixmap pixmap(wi, he);
    pixmap.fill();
    QPainter painter(&pixmap);

    for (uint ii = 0; ii < m_listPoints.size(); ++ii) {
        QPolygonF points = getListPoints(ii, wi, he);

        const Poly& pl = m_listPoints.at(ii);

        painter.setPen(pl.colorLine);
        painter.drawPolyline(points.data(), points.size());

        painter.setPen(pl.colorPoints);
        painter.setBrush(pl.colorPoints);

        for (const QPointF& p: points)
            painter.drawEllipse(p, 2, 2);
    }

    paintPixmap(pixmap);
}

void MyPlot::paintPixmap(const QPixmap& pixmap) {

    auto list = m_scene->items();
    if (list.size() == 0) {
        auto* item = new QGraphicsPixmapItem(pixmap);
        m_scene->addItem(item);
    } else {
        auto* gpi = static_cast<QGraphicsPixmapItem*>(list.first());
        gpi->setPixmap(pixmap);
    }
}

MyPlot::MyPlot(QWidget* parent)
    : QWidget(parent) {

    setAttribute(Qt::WA_DeleteOnClose);

    QScreen* screen = QGuiApplication::primaryScreen();
    auto srect = screen->availableGeometry();

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            {600,400}, srect)
    );

    auto* lt = new QVBoxLayout;
    setLayout(lt);

    m_view = new QGraphicsView;
    m_scene = new QGraphicsScene;
    m_view->setScene(m_scene);

    lt->addWidget(m_view);
}

