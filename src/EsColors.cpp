#include "EsColors.h"
#include <QBasicAtomicInt>
#include <QtAlgorithms>

struct EsColorsPrivate
{
public:
    QBasicAtomicInt ref;
    QVector<QRgb> colors;
    QVector<double> positions;
    QLinearGradient gradient;
    bool hasGeneratedGradient;

    static EsColorsPrivate shared_null;

    EsColorsPrivate();
    ~EsColorsPrivate();
    void fillColors(QRgb *colorArray, int length);
    void fillPositions(double* positions, int length);
    void autoGeneratePosition(int size);
    void checkLength();
};

EsColorsPrivate EsColorsPrivate::shared_null;

static void free(EsColorsPrivate *&d)
{
    delete d;
    d = 0;
}

//-----------------------------------------------------------------------------------------------------

EsColors::EsColors()
    : d(&EsColorsPrivate::shared_null)
{
    d->ref.ref();
}

EsColors::~EsColors()
{
    if (!d)
        return;
    if (!d->ref.deref())
        free(d);
}

EsColors::EsColors(const EsColors &other)
    : d(other.d)
{
    d->ref.ref();
}

EsColors &EsColors::operator=(const EsColors &other)
{
    if (this == &other)
        return *this;

    EsColorsPrivate *o = other.d;
    o->ref.ref();
    if (!d->ref.deref())
        free(d);
    d = o;
    return *this;
}

void EsColors::swap(const EsColors &other)
{
    qSwap(*d, *other.d);
}

EsColors::EsColors(const QVector<QRgb> &colors)
{
    d = new EsColorsPrivate;
    d->ref = 1;
    d->colors = colors;
    d->autoGeneratePosition(colors.size());
}

EsColors::EsColors(QRgb *colorArray, int length)
{
    d = new EsColorsPrivate;
    d->ref = 1;
    d->fillColors(colorArray, length);
    d->autoGeneratePosition(length);
}

EsColors::EsColors(const QVector<QRgb> &colors, const QVector<double> &positions)
{
    d = new EsColorsPrivate;
    d->ref = 1;
    d->colors = colors;
    d->positions = positions;
    d->checkLength();
}

EsColors::EsColors(QRgb *colorArray, double* positions, int length)
{
    d = new EsColorsPrivate;
    d->ref = 1;
    d->fillColors(colorArray, length);
    d->fillPositions(positions, length);
    d->checkLength();
}

EsColors::EsColors(QRgb *colorArray, int length, const QVector<double> &positions)
{
    d = new EsColorsPrivate;
    d->ref = 1;
    d->fillColors(colorArray, length);
    d->positions = positions;
    d->checkLength();
}

QVector<QRgb> EsColors::getColors() const
{
    return d->colors;
}

QVector<double> EsColors::getPositions() const
{
    return d->positions;
}

QLinearGradient EsColors::toGradient() const
{
    if (!d->hasGeneratedGradient)
    {
        d->hasGeneratedGradient = true;
        for (int i = 0; i < d->colors.size(); i++)
            d->gradient.setColorAt(d->positions[i], d->colors[i]);
    }
    return d->gradient;
}

//-----------------------------------------------------------------------------------------------

EsColorsPrivate::EsColorsPrivate()
{
    hasGeneratedGradient = false;
}

EsColorsPrivate::~EsColorsPrivate()
{
    hasGeneratedGradient = false;
}

void EsColorsPrivate::fillColors(QRgb *colorArray, int length)
{
    colors.resize(length);
    for (int i = 0; i < length; i++)
        colors[i] = colorArray[i];
}

void EsColorsPrivate::fillPositions(double *pos, int length)
{
    positions.resize(length);
    for (int i = 0; i < length; i++)
        positions[i] = pos[i];
}

void EsColorsPrivate::autoGeneratePosition(int size)
{
    positions.clear();
    if (size == 1)
        positions.append(0);
    else if (size > 1) {
        double step = 1.0 / (size - 1);
        positions.append(0);
        for (int i = 1; i < size - 1; i++)
            positions.append(step * i);
        positions.append(1);
    }
}

void EsColorsPrivate::checkLength()
{
    if (colors.size() != positions.size())
    {
        int newSize = qMin(colors.size(), positions.size());
        if (colors.size() != newSize)
            colors.resize(newSize);
        if (positions.size() != newSize)
            positions.resize(newSize);
    }
}
