
#include <QLineEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "engine.h"
#include "myPlot.h"
#include "tls.h"

void MainWindow::drawPlot() {

    int cDistrPoints = ui->ed_cntDistrib->text().toInt();
    int cIter = ui->ed_cntIterDistrib->text().toInt();

    cDistrPoints = std::max(100, cDistrPoints);
    cIter = std::max(100, cIter);

    int cFractions = 30;
    int cWins[cFractions] = {};
    int cAll = 0;

    for (int iStp = 0; iStp < cDistrPoints; ++iStp) {
        Deck deck;
        ListCards myHand = deck.takeRandomCount(2);
        ListCards desk = deck.takeRandomCount(3);

        ResProb prob = ::calcProbability(deck, myHand, desk, {}, cIter);

        double p = prob.probWin();
        int iPos = lround(floor(p * cFractions));

        if (iPos < 0) iPos = 0;
        if (iPos >= cFractions) iPos = cFractions-1;

        ++cWins[iPos];
        ++cAll;

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
    double probWinCancel = 1.;

    QVector<QPointF> points;

    double suProb = 0.;
    double maProb = 0.;

    for (int ii = 0; ii < cFractions; ++ii) {
        double p = DIV(cWins[ii], cAll);
        double x = DIV(ii + 0.5, cFractions);
        points += {x, p};

        if (suProb + p > probCancel && probWinCancel >= 1.)
            probWinCancel =
            DIV(ii, cFractions) +
            1. / cFractions * (probCancel - suProb) / p;

        suProb += p;
        maProb = std::max(maProb, p);
    }

    ui->lb_newProbWinCancel->setText(
    "Probability of win for cancel: "+
    fmt(probWinCancel, 2, 1));

    QPolygonF pgCancel;
    pgCancel += {probWinCancel, maProb * -0.1};
    pgCancel += {probWinCancel, maProb * 1.1};

    auto* plot = new MyPlot;

    plot->appendPoints({{0,0},{1,0}}, QColorConstants::Gray);

    double x = 0.;
    for (int ii = 0; ii < 3; ++ii, x += 0.5) {
        QPolygonF pgOrd1;
        pgOrd1 += {x, maProb * -0.1};
        pgOrd1 += {x, maProb * 1.1};
        plot->appendPoints(pgOrd1, QColorConstants::Gray);
    }

    plot->appendPoints(points, QColorConstants::Black);

    if (probCancel > 0.)
        plot->appendPoints(pgCancel, QColorConstants::Red);

    plot->show();
}

void MainWindow::on_bt_distribution_clicked() {
    drawPlot();
}
