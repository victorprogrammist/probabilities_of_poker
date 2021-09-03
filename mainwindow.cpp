#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QBoxLayout>
#include <QKeyEvent>

#include <vector>
#include <cstdlib>
#include <ctime>

#include "combination.h"
#include "card.h"
#include "deck.h"
#include "engine.h"
#include "tls.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(NULL));

    auto mkRe = [&](const QString& s) {
        return QRegularExpression(s); };

    m_valid_oneNum.setRegularExpression(mkRe("^[5-7]$"));

    ui->ed_qtyRandGen->setValidator(&m_valid_oneNum);
    ui->ed_qtyRandGen->setText("5");

    m_valid_cardNumNames.setRegularExpression(mkRe(
        "^(?:(?:[2-9][1-4]|1[0-4][1-4])[-+ ])+"));

    ui->ed_numCards->setValidator(&m_valid_cardNumNames);

    m_valid_cardNames.setRegularExpression(mkRe(
        "^(?:[2-9TJQKAtjqka][cdhsCDHS]\\s)+"));

    ui->ed_cards->setValidator(&m_valid_cardNames);

    m_valid_probWinCancel.setRange(0.001, 0.9, 3);
    ui->ed_probWinCancel->setValidator(&m_valid_probWinCancel);
    ui->ed_probWinCancel->setText("0.4");

    connect(ui->bt_generateRandom, &QPushButton::clicked,
        [this] { generateRandomVariant(); });

    connect(ui->bt_recalcProbability, &QPushButton::clicked,
        [this] { calcProbability(true); });

    generateRandomVariant();

    ui->ed_numCards->installEventFilter(this);
    ui->ed_cards->installEventFilter(this);
    ui->ed_probWinCancel->installEventFilter(this);

    ui->tabWidget->setCurrentWidget(ui->tab_calc);

    //***************************************
    ui->lb_newProbWinCancel->setText("Probability of win for cancel:");
    m_valid_probCancel.setRange(0., 0.99, 2);
    ui->ed_probCancel->setValidator(&m_valid_probCancel);
    ui->ed_probCancel->setText("0.33");

    m_valid_iterate.setBottom(100);

    ui->ed_cntDistrib->setValidator(&m_valid_iterate);
    ui->ed_cntDistrib->setText("10000");
    ui->ed_cntIterDistrib->setValidator(&m_valid_iterate);
    ui->ed_cntIterDistrib->setText("1000");

    ui->ed_distribFractions->setText("30");
    ui->ed_distribFractions->setValidator(&m_valid_iterate);

    //***************************************
    ui->ed_simpleIter->setText("50000");
    ui->ed_realIter->setText("200");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* w, QEvent* ev) {

    bool isEd =
        w == ui->ed_numCards || w == ui->ed_cards
        || w == ui->ed_probWinCancel;

    if (ev->type() == QEvent::FocusOut && isEd)
        calcProbability();

    if (ev->type() == QEvent::KeyPress && isEd) {
        QKeyEvent* key = static_cast<QKeyEvent*>(ev);
        if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) )
            calcProbability();
    }

    return false;
}

void MainWindow::on_ed_numCards_textEdited(const QString &arg1) {

    auto list = parseNumericCards(arg1);
    QString s = listNamesCards(list);

    if (s == cardsLastEdited)
        return;

    cardsLastEdited = s;
    ui->ed_cards->setText(s);
}

void MainWindow::on_ed_cards_textEdited(const QString &arg1) {

    auto list = parseCards(arg1);
    QString s = listNamesCards(list);
    if (s == cardsLastEdited)
        return;

    cardsLastEdited = s;
    ui->ed_numCards->setText(listNumericNamesCards(list));
}

//********************************************************************

void MainWindow::generateRandomVariant() {

    int qty = getEditText(ui->ed_qtyRandGen, "5").toInt();
    qty = std::max(5, std::min(7, qty));

    Deck deck;

    ListCards list = deck.takeRandomCount(qty);
    QString s = listNamesCards(list);

    ui->ed_cards->setText(s);
    ui->ed_numCards->setText(listNumericNamesCards(list));

    cardsLastEdited = s;

    calcProbability();
}

void MainWindow::calcProbability(bool force) {

    double probWinCancel = getEditText(ui->ed_probWinCancel, "0.4").toDouble();

    if (!force)
        if (cardsLastEdited == cardsLastCalculated)
            if (probWinCancel == lastCalculatedProbCancel)
                return;

    int cSimpIter = getEditText(ui->ed_simpleIter, "50000").toInt();
    cSimpIter = std::max(cSimpIter, 100);

    int cRealIter = getEditText(ui->ed_realIter, "200").toInt();
    cRealIter = std::max(cRealIter, 50);

    cardsLastCalculated = cardsLastEdited;
    lastCalculatedProbCancel = probWinCancel;

    vector<IdxCard> cards = parseCards(cardsLastEdited);

    auto resProb = calcAbstractProbability(cards, cSimpIter);

    QString sRes =
    "Win: " + fmt(resProb.probWin(),2,1)+",  Loss: " + fmt(resProb.probLoss(),2,1);

    ui->lb_abstractProbability->setText(sRes);

    auto resPseudo = calcPseudoRealProbability(cards, probWinCancel, cRealIter, cRealIter);

    QString sRes2 =
    "Win: " + fmt(resPseudo.probWin(),2,1)+",  Loss: " + fmt(resPseudo.probLoss(),2,1);

    ui->lb_pseudoRealProbability->setText(sRes2);
}



