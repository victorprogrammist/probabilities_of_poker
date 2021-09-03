#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <QDoubleValidator>
#include <QIntValidator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void generateRandomVariant();
    void calcProbability(bool force = false);
    void drawPlot();

    QVector<QPointF> calcPlot();

private slots:
    void on_ed_numCards_textEdited(const QString &arg1);
    void on_ed_cards_textEdited(const QString &arg1);
    void on_bt_distribution_clicked();

private:
    Ui::MainWindow *ui;

    QString cardsLastCalculated;
    QString cardsLastEdited;
    double lastCalculatedProbCancel = 0;

    bool eventFilter(QObject* w, QEvent* ev) override;

    QRegularExpressionValidator m_valid_oneNum;
    QRegularExpressionValidator m_valid_cardNames;
    QRegularExpressionValidator m_valid_cardNumNames;
    QDoubleValidator m_valid_probWinCancel;

    QDoubleValidator m_valid_probCancel;
    QIntValidator m_valid_iterate;
};

#endif // MAINWINDOW_H
