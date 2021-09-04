
#ifndef __DISTRIB_H__
#define __DISTRIB_H__

#include <QPointF>
#include <QVector>
#include "fdiv.h"

struct Distrib {

    int cFractions = 0;

    // для случая, когда результат это вероятность,
    //   здесь суммируются просто единицы случаев.
    double *arSuPoints = nullptr;
    double suAll = 0;

    Distrib(int cFr)
        : cFractions(cFr)
        , arSuPoints(new double[cFr]) {}

    ~Distrib() { delete[] arSuPoints; }

    double getAreaOfPoint(int iPoint) const {
        return arSuPoints[iPoint] / suAll; }

    double getDensity(int iPoint) const {
        // area / distX => area / (1. / cFractions)
        return getAreaOfPoint(iPoint) * cFractions; }

    double getProbabilityX(int iPoint) const {
        return FDIV(iPoint + 0.5, cFractions); }

    QPointF getPoint(int iPoint) const {
        return {getProbabilityX(iPoint), getDensity(iPoint)}; }

    QVector<QPointF> getAllPoints() const {
        QVector<QPointF> points;
        for (int ii = 0; ii < cFractions; ++ii)
            points += getPoint(ii);
        return points; }

    int indexFraction(double pX) const;
    void addValue(double pX) { addValue(pX, 1.); }
    void addValue(double pX, double v);

    double probabilityX_byArea(double area) const;
    double maxDensity() const;

    // в отличиии от getDensity считает не на индекс деления,
    //   а на дробную величину оси X.
    //   И для случаев pX не выравненных на getProbabilityX,
    //   расчет выполняется взятием двух ближайших ячеек
    //   и расчетом пропорции.
    double getDensityByProbabilityX(double pX) const;
};

#endif
