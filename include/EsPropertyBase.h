#ifndef ESPROPERTYBASE_H
#define ESPROPERTYBASE_H

#include <QObject>
#include <QSet>
#include <QLatin1String>
#include <QMap>
#include <QScopedPointer>

#include "estreepackage_global.h"
#include "EsPropertyInterface.h"
#include "QtPropertyCache.h"

class QtProperty;
class QtVariantProperty;
class QtTreePropertyBrowser;
class EsPropertyTabWidget;
class QtVariantPropertyManager;
class QtVariantEditorFactory;

class ESTREEPACKAGESHARED_EXPORT EsPropertyBase : public QObject, public EsPropertyInterface
{
    Q_OBJECT
public:
    enum BackgroundColor
    {
        MultiColor = -2,
        NoneColor = -1,
        //
        Yellow = 0,
        Green,
        Purple,
        Cyan,
        Pink,
        Orange,
        //
        ColorCount
    };
    static void setDefaultBackground(BackgroundColor bk);

    explicit EsPropertyBase(QObject *parent = 0);
    virtual ~EsPropertyBase();

    virtual void update();
    virtual void retrieve();
    virtual bool initialize(SSI_Object* obj, EsPropertyTabWidget* editor);

    void addProperties(EsPropertyTabWidget* editor);

    // if want to use an external/existing browser, call setUseExistingBrowser before calling addProperties
    void setUseExistingBrowser(QtTreePropertyBrowser* browser);
    bool useExistingBrowser() const;

    // if want to use an external browser and attach this to another group property as parent,
    // call setParentProperty before calling addProperties
    void setParentProperty(QtProperty* parent);
    QtProperty* getParentProperty() const;
    bool hasParentProperty() const;
protected:
    // Programming style: use id to identify property
    void addProperty(QtProperty* prop, const QString& id, QtProperty* parentProp);
    void addProperty(QtProperty* prop, const QString& id, QtTreePropertyBrowser* browser);

    // Programming style: use member variable to identify property
    void addProperty(QtProperty* prop, QtProperty* parentProp);
    void addProperty(QtProperty* prop, QtTreePropertyBrowser* browser);

    QtProperty* getProperty(QString id) const;
    QtVariantProperty* getVariantProperty(QString id) const;

    QString getId(QtProperty* property) const;

    void removeProperty(QtProperty* property); // remove property from inner set, and delet it
    void removeProperty(QString id); // remove property from inner set, and delet it

    QtProperty* takeProperty(QtProperty* property); // remove property from inner set, but not delet it
    QtProperty* takeProperty(QString id); // remove property from inner set, but not delet it
private:
    // for clearing your own stuff that is created by derived class. NOTE: do not use browserBase.
    virtual void onBrowserCleared(const QSet<QtTreePropertyBrowser *> &browsers);

    // update displaying properties
    virtual void onUpdate() = 0;

    // create proeprty and add to  QtTreePropertyBrowser
    virtual void initializingProperty(EsPropertyTabWidget *editor, QtTreePropertyBrowser* browser, int indexBrowser) = 0;

    // value of property has been changed by user, you need set the value to your object according to property
    virtual void onValueChanged(QtProperty *property, const QVariant &val) = 0;

    virtual void onSetDefault();
    virtual void onGetDefault();

    void updateChild();
private slots:
    void browserCleared(QSet<QtTreePropertyBrowser*> const& browsers, EsPropertyTabWidget* editor);
    void valueChanged(QtProperty *property, const QVariant &val);
    void needApplyChangeToData(int tabIndex);
    void applyPropertyToDefault(int tabIndex);
    void getDefaultProperty(int tabIndex);
protected:
    SSI_Object* m_obj;
    QScopedPointer<QtVariantPropertyManager> propManager;
private:
    QtTreePropertyBrowser* browserExternal;
    QtTreePropertyBrowser* browserBase;
    QtProperty* parentProperty;

    QScopedPointer<QtVariantEditorFactory> editorFactory;
    QMap<QString, QtProperty*> idToProperty;
    QMap<QtProperty*, QString> propertyToId;
    QSet<QtProperty*> noneIdPropeties;

    int indexOfBrowserBase;
    QtPropertyCache cachedProperty;
    EsPropertyTabWidget *editor;
    bool isUpdating;
};

#endif // ESPROPERTYBASE_H
