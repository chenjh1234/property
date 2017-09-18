#ifndef ESPROPERTYINTERFACE_H
#define ESPROPERTYINTERFACE_H

#include "estreepackage_global.h"

class SSI_Object;
class EsPropertyTabWidget;

class ESTREEPACKAGESHARED_EXPORT EsPropertyInterface
{
public:
    virtual ~EsPropertyInterface();

    /**
     * @brief initialize property, or create sub-item
     * @param tree  the tree node that the property associate with
     * @param editor the tab-widget displaying properties
     * @return succeed: return true; fail: return false.
     */
    virtual bool initialize(SSI_Object* obj, EsPropertyTabWidget* editor) = 0;

    /**
     * @brief update displaying properties
     */
    virtual void update() = 0;

    /**
     * @brief retrieve property and set the target object
     */
    virtual void retrieve() = 0;
};

#endif // ESPROPERTYINTERFACE_H
