#include "EsPropertyGroupBase.h"

EsPropertyGroupBase::EsPropertyGroupBase()
{
}

EsPropertyGroupBase::~EsPropertyGroupBase()
{
    qDeleteAll(children_);
}

bool EsPropertyGroupBase::initialize(SSI_Object *obj, EsPropertyTabWidget *editor)
{
    foreach (EsPropertyInterface* prop, children_) {
        if (!prop->initialize(obj, editor))
            return false;
    }
    return true;
}

void EsPropertyGroupBase::update()
{
    foreach (EsPropertyInterface* prop, children_) {
        prop->update();
    }
}

void EsPropertyGroupBase::retrieve()
{
    foreach (EsPropertyInterface* prop, children_) {
        prop->retrieve();
    }
}

bool EsPropertyGroupBase::hasChild() const
{
    return !children_.isEmpty();
}

void EsPropertyGroupBase::addChild(EsPropertyInterface *child)
{
    children_.append(child);
}

QVector<EsPropertyInterface *> EsPropertyGroupBase::children() const
{
    return children_;
}
