#ifndef ESPROPERTYGROUPBASE_H
#define ESPROPERTYGROUPBASE_H

#include "EsPropertyInterface.h"
#include <QVector>

/**
 * @brief The EsPropertyGroupBase class, manager sub property object, add
 * a new sub property using addChild(). It will auto delete all chilren when destruct.
 */
class ESTREEPACKAGESHARED_EXPORT EsPropertyGroupBase : public EsPropertyInterface
{
public:
    EsPropertyGroupBase();
    virtual ~EsPropertyGroupBase();

    virtual bool initialize(SSI_Object* obj, EsPropertyTabWidget* editor);
    virtual void update();
    virtual void retrieve();

    bool hasChild() const;
    void addChild(EsPropertyInterface* child);
    QVector<EsPropertyInterface*> children() const;
protected:
    QVector<EsPropertyInterface*> children_;
};

#endif // ESPROPERTYGROUPBASE_H
