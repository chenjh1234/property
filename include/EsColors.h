#ifndef ESCOLORS_H
#define ESCOLORS_H
#include "estreepackage_global.h"
#include <QVector>
#include <QRgb>
#include <QMetaType>
#include <QLinearGradient>

/** manage colors or color array, and can generate a QLinearGradient
 * for displaying.  Implicit shared memory , reduse copying data.
*/
class ESTREEPACKAGESHARED_EXPORT EsColors
{
public:
    EsColors();
    ~EsColors();
    EsColors(const EsColors& other);
    EsColors& operator=(const EsColors& other);

    void swap(const EsColors& other);

    explicit EsColors(const QVector<QRgb>& colors);
    EsColors(QRgb* colorArray, int length);

    /**
     * @param colors
     * @param positions   Creates a stop point at the given position with the
     *  given color. The given position must be in the range 0 to 1
     */
    EsColors(const QVector<QRgb>& colors, const QVector<double>& positions);

    /**
     * @param colorArray   set the colors.
     * @param positions   Creates a stop point at the given position with the
     *  given color. The given position must be in the range 0 to 1.
     * @param length   the count of colors and positions.
     */
    EsColors(QRgb* colorArray, double* positions, int length);

    EsColors(QRgb* colorArray, int length, const QVector<double>& positions);

    QVector<QRgb> getColors() const;
    QVector<double> getPositions() const;
    QLinearGradient toGradient() const;
private:
    class EsColorsPrivate* d;
};

Q_DECLARE_METATYPE(EsColors)

#endif // ESCOLORS_H
