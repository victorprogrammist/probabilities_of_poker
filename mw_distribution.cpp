
#include <QLineEdit>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "distrib.h"
#include "engine.h"
#include "myPlot.h"
#include "tls.h"

void MainWindow::drawPlot() {

    int cDistrPoints = ui->ed_cntDistrib->text().toInt();
    cDistrPoints = std::max(100, cDistrPoints);

    int cIter = ui->ed_cntIterDistrib->text().toInt();
    cIter = std::max(100, cIter);

    int cFractions = ui->ed_distribFractions->text().toInt();
    cFractions = std::min(100, std::max(3, cFractions));

    Distrib distrib(cFractions);

    for (int iStp = 0; iStp < cDistrPoints; ++iStp) {
        Deck deck;
        ListCards myHand = deck.takeRandomCount(2);
        ListCards desk = deck.takeRandomCount(3);

        ResProb prob = ::calcProbability(deck, myHand, desk, {}, cIter);

        double p = prob.probWin();
        distrib.addValue(p);

#if 1
        if (iStp % 100 == 0)
            qDebug().noquote()
            << iStp
            << "HAND"
            << listNamesCards(myHand)
            << "DESK"
            << listNamesCards(desk)
            << "WIN"
            << fmt(prob.probWin(), 2, 1)
            << "LOSS"
            << fmt(prob.probLoss(), 2, 1);
#endif

        deck.recallList(myHand);
        deck.recallList(desk);
        deck.checkFullAss();
   }

    //***************************************************************************

    double probCancel = getEditText(ui->ed_probCancel, "0.3").toDouble();

    QVector<QPointF> points = distrib.getAllPoints();
    double maDen = distrib.maxDensity();
    double probWinCancel = distrib.probabilityX_byArea(probCancel);

    ui->lb_newProbWinCancel->setText(
    "Probability of win for cancel: "+
    fmt(probWinCancel, 2, 1));

    QPolygonF pgCancel;
    pgCancel += {probWinCancel, maDen * -0.1};
    pgCancel += {probWinCancel, maDen * 1.1};

    auto* plot = new MyPlot;

    plot->appendPoints({{0,0},{1,0}}, QColorConstants::Gray);

    double x = 0.;
    for (int ii = 0; ii < 3; ++ii, x += 0.5) {
        QPolygonF pgOrd1;
        pgOrd1 += {x, maDen * -0.1};
        pgOrd1 += {x, maDen * 1.1};
        plot->appendPoints(pgOrd1, QColorConstants::Gray);
    }

    plot->appendPoints(points, QColorConstants::Black);

    if (probCancel > 0.)
        plot->appendPoints(pgCancel, QColorConstants::Red);

    QTimer::singleShot(1, [this,plot] {
        plot->show();
        plot->repaintPlot(); });
}

void MainWindow::on_bt_distribution_clicked() {
    drawPlot();
}

