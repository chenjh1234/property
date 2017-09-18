#ifndef ESPROPERTYCOLORTEMPLATE_H
#define ESPROPERTYCOLORTEMPLATE_H

#include "EsPropertyBase.h"
#include <QRgb>

class EsPropertyColorTemplate : public EsPropertyBase
{
    Q_OBJECT
public:
    EsPropertyColorTemplate(QString title, QList<QColor> *colors);

    virtual void onUpdate();
    virtual void initializingProperty(EsPropertyTabWidget *editor,
                                      QtTreePropertyBrowser* browser,
                                      int indexBrowser);
    virtual void onValueChanged(QtProperty *property, const QVariant &val);
protected:
    QList<QColor>* getColors() const { return colors; }

    void resetTo(const QList<QColor>& newColors);
private:
    void addOneColorProperty(QRgb color, QString id, QtProperty* group = 0);
private slots:
    void openPathTriggered(QtVariantProperty* property);
private:
    QString title;
    QList<QColor>* colors;
};

#endif // ESPROPERTYCOLORTEMPLATE_H
