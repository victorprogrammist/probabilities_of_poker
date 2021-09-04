
#include "distrib.h"

double Distrib::getDensityByProbabilityX(double pX) const {

    int iPos = indexFraction(pX);

    double ptProbX = getProbabilityX(iPos);

    auto fn = [&](int i1, int i2) -> double {
        double x1 = getProbabilityX(i1);
        double x2 = getProbabilityX(i2);
        double d1 = getDensity(i1);
        double d2 = getDensity(i2);
        return (pX - x1) / (x2 - x1) * (d2 - d1) + d1;
    };

    if (pX < ptProbX) {
        if (!iPos)
            return getDensity(iPos);
        return fn(iPos-1, iPos);
    }

    if (pX > ptProbX) {
        if (iPos == cFractions-1)
            return getDensity(iPos);
        return fn(iPos, iPos+1);
    }

    return getDensity(iPos);
}

int Distrib::indexFraction(double pX) const {
    // pX: 0..1
    int iPos = lround(floor(pX * cFractions));

    if (iPos < 0) iPos = 0;
    if (iPos >= cFractions) iPos = cFractions-1;

    return iPos;
}

void Distrib::addValue(double pX, double v) {

    arSuPoints[indexFraction(pX)] += v;
    suAll += v;
}

double Distrib::probabilityX_byArea(double area) const {
    // area: 0..1

    if (area <= 0)
        return 0.;

    double suArea = 0;

    for (int ii = 0; ii < cFractions; ++ii) {
        double p = getAreaOfPoint(ii);

        if (suArea + p > area)
            return
            FDIV(ii, cFractions) + 1. / cFractions * (area - suArea) / p;

        suArea += p;
    }

    return 1.;
}

double Distrib::maxDensity() const {
    double maProb = 0;
    for (int ii = 0; ii < cFractions; ++ii)
        maProb = std::max(maProb, getDensity(ii));
    return maProb;
}
