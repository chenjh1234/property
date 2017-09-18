#include "EsPropertyBase.h"
#include "qttreepropertybrowser.h"
#include "EsPropertyTabWidget.h"
#include "qtvariantproperty.h"
#include <qdebug.h>

#ifndef _countof
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

static EsPropertyBase::BackgroundColor s_defaultBkColor = EsPropertyBase::NoneColor;

static void setTreeBrowserBackgroundColor(QtTreePropertyBrowser* browser);

static void setTreeBrowserBackgroundColor(QtTreePropertyBrowser *browser)
{
    static const QColor colors[] = {
        QColor(255,255,191),
        QColor(191,255,191),
        QColor(234,191,255),
        QColor(199,255,255),
        QColor(255,191,239),
        QColor(255,230,191),
    };
    static const int colorCount = _countof(colors);

    if (s_defaultBkColor == EsPropertyBase::NoneColor)
        return;

    const QList<QtBrowserItem*> items = browser->topLevelItems();
    if (items.isEmpty())
        return;

    int itemCount = items.count();
    if (s_defaultBkColor == EsPropertyBase::MultiColor)
    {
        for (int i = 0, idxColor = 0; i < itemCount; i++) {
            browser->setBackgroundColor(items.at(i), colors[idxColor % colorCount]);
            if (items.at(i)->hasChild())
                idxColor++;
            else if (i + 1 < itemCount && items.at(i + 1)->hasChild())
                idxColor++;
        }
    }
    else
    {
        for (int i = 0; i < itemCount; i++)
            browser->setBackgroundColor(items.at(i), colors[s_defaultBkColor]);
    }
}


void EsPropertyBase::setDefaultBackground(EsPropertyBase::BackgroundColor bk)
{
    if (bk < ColorCount && bk >= MultiColor)
        s_defaultBkColor = bk;
}

EsPropertyBase::EsPropertyBase(QObject *parent)
    : QObject(parent)
    , browserExternal(0)
    , browserBase(0)
    , parentProperty(0)
    , m_obj(0)
    , indexOfBrowserBase(0)
    , editor(0)
    , isUpdating(false)
{
}

EsPropertyBase::~EsPropertyBase()
{
}

void EsPropertyBase::update()
{
    isUpdating = true;
    onUpdate();
    isUpdating = false;
    updateChild();
}

void EsPropertyBase::retrieve()
{
    foreach (QtProperty* property, propertyToId.keys())
        onValueChanged(property, propManager->value(property));
    foreach (QtProperty* property, noneIdPropeties)
        onValueChanged(property, propManager->value(property));
    foreach (EsPropertyBase* prop, this->findChildren<EsPropertyBase*>())
        prop->retrieve();
}

bool EsPropertyBase::initialize(SSI_Object *obj, EsPropertyTabWidget *editor)
{
    this->m_obj = obj;
    addProperties(editor);
    return true;
}

void EsPropertyBase::updateChild()
{
    foreach (EsPropertyBase* prop, this->findChildren<EsPropertyBase*>())
        prop->update();
}

QtProperty *EsPropertyBase::getProperty(QString id) const
{
    return idToProperty[id];
}

QtVariantProperty *EsPropertyBase::getVariantProperty(QString id) const
{
    return static_cast<QtVariantProperty*>(idToProperty[id]);
}

QString EsPropertyBase::getId(QtProperty *property) const
{
    return propertyToId[property];
}

void EsPropertyBase::removeProperty(QtProperty *property)
{
    if (QtProperty* tmp = takeProperty(property))
        delete tmp;
}

void EsPropertyBase::removeProperty(QString id)
{
    removeProperty(getProperty(id));
}

QtProperty *EsPropertyBase::takeProperty(QtProperty *property)
{
    if (!property)
        return 0;

    if (noneIdPropeties.contains(property))
        noneIdPropeties.remove(property);
    if (propertyToId.contains(property))
    {
        QString id = propertyToId.value(property);
        propertyToId.remove(property);
        idToProperty.remove(id);
    }
    return property;
}

QtProperty *EsPropertyBase::takeProperty(QString id)
{
    return takeProperty(getProperty(id));
}

void EsPropertyBase::addProperties(EsPropertyTabWidget *editor)
{
    this->editor = editor;
    if (!propManager) {
        propManager.reset(new QtVariantPropertyManager);
        connect(propManager.data(), SIGNAL(valueChanged(QtProperty*, const QVariant&)),
                this, SLOT(valueChanged(QtProperty*, const QVariant&)));
    }
    if (!editorFactory) {
        editorFactory.reset(new QtVariantEditorFactory);
    }
    if (!browserExternal) {
        browserBase = new QtTreePropertyBrowser(editor);
        browserBase->setRootIsDecorated(false);
    }
    else
        browserBase = browserExternal;
    browserBase->setFactoryForManager(propManager.data(), editorFactory.data());
    int index = (browserExternal ? editor->indexOfBrowser(browserBase) :
                                   editor->addBrowser(browserBase));

    propManager->blockSignals(true);
    //{begin todo list
    if (!browserExternal) {
        editor->setAutoUpdateData(true, index);
        editor->setAutoUpdateUIVisible(true, index);
    }
    indexOfBrowserBase = index;
    initializingProperty(editor, browserBase, index);
    if (!useExistingBrowser()) {
        setTreeBrowserBackgroundColor(browserBase);
        browserBase->setPropertiesWithoutValueMarked(true);
    }
    //}end
    propManager->blockSignals(false);

    connect(editor, SIGNAL(browserCleared(const QSet<QtTreePropertyBrowser*> &, EsPropertyTabWidget*)),
            this, SLOT(browserCleared(const QSet<QtTreePropertyBrowser *> &, EsPropertyTabWidget*)),
            Qt::UniqueConnection);
    connect(editor, SIGNAL(needApplyChangeToData(int)),
            this, SLOT(needApplyChangeToData(int)), Qt::UniqueConnection);
    connect(editor, SIGNAL(applyPropertyToDefault(int)),
            this, SLOT(applyPropertyToDefault(int)), Qt::UniqueConnection);
    connect(editor, SIGNAL(getDefaultProperty(int)),
            this, SLOT(getDefaultProperty(int)), Qt::UniqueConnection);
}

void EsPropertyBase::browserCleared(const QSet<QtTreePropertyBrowser *> &browsers, EsPropertyTabWidget *editor)
{
    if (browsers.contains(browserBase))
    {
        if (!browserExternal)
            browserBase->deleteLater();
        browserBase = 0;
        browserExternal = 0;
        parentProperty = 0;
    }
    onBrowserCleared(browsers);
    if (!browserBase)
    {
        if (editor)
            this->disconnect(editor);
        editorFactory.reset();
        propManager.reset();
        idToProperty.clear();
        propertyToId.clear();
        noneIdPropeties.clear();
        this->editor = 0;

        disconnect(editor, SIGNAL(browserCleared(const QSet<QtTreePropertyBrowser*> &, EsPropertyTabWidget*)),
                this, SLOT(browserCleared(const QSet<QtTreePropertyBrowser *> &, EsPropertyTabWidget*)));
        disconnect(editor, SIGNAL(needApplyChangeToData(int)),
                this, SLOT(needApplyChangeToData(int)));
        disconnect(editor, SIGNAL(applyPropertyToDefault(int)),
                this, SLOT(applyPropertyToDefault(int)));
        disconnect(editor, SIGNAL(getDefaultProperty(int)),
                this, SLOT(getDefaultProperty(int)));
    }
}

void EsPropertyBase::onBrowserCleared(const QSet<QtTreePropertyBrowser *> &browsers)
{

}

void EsPropertyBase::onSetDefault()
{

}

void EsPropertyBase::onGetDefault()
{

}

void EsPropertyBase::setUseExistingBrowser(QtTreePropertyBrowser *browser)
{
    Q_ASSERT(browser);
    browserExternal = browser;
}

bool EsPropertyBase::useExistingBrowser() const
{
    return browserExternal != 0;
}

void EsPropertyBase::setParentProperty(QtProperty *parent)
{
    Q_ASSERT(parent);
    this->parentProperty = parent;
}

QtProperty *EsPropertyBase::getParentProperty() const
{
    return parentProperty;
}

bool EsPropertyBase::hasParentProperty() const
{
    return parentProperty != 0;
}

void EsPropertyBase::valueChanged(QtProperty *property, const QVariant &val)
{
    if (isUpdating)
        return;
    if (!propertyToId.contains(property) && !noneIdPropeties.contains(property))
        return;
    if (editor && !editor->isAutoUpdateData(indexOfBrowserBase))
    {
        cachedProperty.insert(property, val);
        return;
    }
#ifdef QT_DEBUG
    QString id = propertyToId[property];
    qDebug() << id << ": " << val;
#endif
    onValueChanged(property, val);
}

void EsPropertyBase::needApplyChangeToData(int tabIndex)
{
    if (tabIndex == indexOfBrowserBase && !cachedProperty.isEmpty())
    {
        foreach (QtProperty* property, cachedProperty.properties()) {
            onValueChanged(property, cachedProperty.getValue(property));
        }
        cachedProperty.clear();
    }
}

void EsPropertyBase::applyPropertyToDefault(int tabIndex)
{
    if (tabIndex == indexOfBrowserBase)
        onSetDefault();
}

void EsPropertyBase::getDefaultProperty(int tabIndex)
{
    if (tabIndex == indexOfBrowserBase)
        onGetDefault();
}

void EsPropertyBase::addProperty(QtProperty *prop, const QString &id, QtProperty *parentProp)
{
    idToProperty[id] = prop;
    propertyToId[prop] = id;
    if (prop->statusTip().isEmpty())
        prop->setStatusTip(prop->propertyName());
    if (parentProp)
        parentProp->addSubProperty(prop);
}

void EsPropertyBase::addProperty(QtProperty *prop, const QString &id, QtTreePropertyBrowser *browser)
{
    idToProperty[id] = prop;
    propertyToId[prop] = id;
    if (prop->statusTip().isEmpty())
        prop->setStatusTip(prop->propertyName());
    browser->addProperty(prop);
}

void EsPropertyBase::addProperty(QtProperty *prop, QtProperty *parentProp)
{
    noneIdPropeties.insert(prop);
    if (prop->statusTip().isEmpty())
        prop->setStatusTip(prop->propertyName());
    if (parentProp)
        parentProp->addSubProperty(prop);
}

void EsPropertyBase::addProperty(QtProperty *prop, QtTreePropertyBrowser *browser)
{
    noneIdPropeties.insert(prop);
    if (prop->statusTip().isEmpty())
        prop->setStatusTip(prop->propertyName());
    browser->addProperty(prop);
}
