 

#ifndef QTVARIANTPROPERTY_H
#define QTVARIANTPROPERTY_H

#include "qtpropertybrowser.h"
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QObject>

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

typedef QMap<int, QIcon> QtIconMap;

class QtVariantPropertyManager;
class QtPathPropertyManager;
class QtCustomPropertyManager;

// predefined attribute name for user, so you can use AttrName::maximum
// instead of magic numeber like "maximum",
// see class QtVariantProperty/QtVariantPropertyManager's setter/getter method setAttribute/attributeValue
class  AttrName
{
public:
    static const QString minimum; // set property's minimum.
    static const QString maximum; // set property's maximum.
    static const QString singleStep; // set single step value when changing  property's value by QSpinBox etc.
    static const QString decimals; // set the number decimals of double value that is displaying in UI,
                                                 // (has no effect on editing and data stored in memery).
    static const QString regExp; // set regular expression to format QVariant::String property.
    static const QString constraint; // constraint attribute, to restrict the area of rectangle property.
    static const QString enumIcons; // set icons that associated with enum value.
                                                    // type is QtIconMap (QMap : enum value -> QIcon).
    static const QString enumNames; // set the enum name that is dispaying in QComboBox.
    static const QString enumValues; // set the enum value.  By default, it set or return 0, 1, 2, etc
                                                     // corresponding to QComboBox's item from top to bottom. If you have
                                                     // diffrent setting, set this attribute to property. The type is QVariantList,
                                                     // each item in QVariantList is integer.
    static const QString flagNames; // set the flag name displaying in UI/QCheckBox.
};

// redefine type id because of QVariant::Bool etc conflict with GeoEast's code
// you can use QtVariant::xxx instead of QVariant::xxx,
// &  use QtVariant::xxx() for short instead of QtVariantProperty::xxx()
class  QtVariant
{
public:
    static const int Boolean;
    static const int Double;
    static const int Color;
    static const int Int;
    static const int String;
    static const int Char;

    static const int Rect;
    static const int RectF;
    static const int Point;
    static const int PointF;
    static const int Size;
    static const int SizeF;

    static const int Font;
    static const int Locale;
    static const int Cursor;
    static const int KeySequence;
    static const int SizePolicy;
    static const int DateTime;
    static const int Date;
    static const int Time;

    static int sliderDoubleTypeId();    // double value type of property,  and use slider to edit value

    static int enumTypeId();               // QComboBox editor, for enum type
    static int iconMapTypeId();           // QtIconMap, use in enum type, set every icon associate with enum value,  QMap£ºenum value -> QIcon
    static int flagTypeId();                 // a group QCheckBox editor, value type is int, use bits as flags
    static int groupTypeId();              // represent a group properties, can just set a title for this group property
    static int groupWithTextTypeId();   // represent a group properties, can set a title for this group property
                                                      // and set a text value that associated with sub-properies

    static int pathTypeId();                // use for opening file, folder, or popup dialog and so on,
                                                    // provided a button and you need connect to signal openPathTriggered() for clicked event

    static int customTypeId(); // custom type of property, can set value of QImage, QPixmap, EsColors, QRgb to property.
                                  // provided 2 button and you need connect to signal customOpenTriggered() for clicked event,
                                  // connect to signal customPopupTriggered() for popup event

    static int buttonTypeId(); // only a button for property. connect to signal buttonClicked()

    // 3 below does not implement now, just provide a button and emit signal openPathTriggered() when clicked
    static int gradientColorTypeId();   // gradient color
    static int colorTemplateTypeId();  // Color Template Type
    static int materialTypeId();           // material type
};

class  QtVariantProperty : public QtProperty
{
public:
    ~QtVariantProperty();
    QVariant value() const;
    QVariant attributeValue(const QString &attribute) const;
    int valueType() const;
    int propertyType() const;

    void setValue(const QVariant &value);
    void setAttribute(const QString &attribute, const QVariant &value);
protected:
    QtVariantProperty(QtVariantPropertyManager *manager);
private:
    friend class QtVariantPropertyManager;
    class QtVariantPropertyPrivate *d_ptr;
};

class  QtVariantPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    QtVariantPropertyManager(QObject *parent = 0);
    ~QtVariantPropertyManager();

    virtual QtVariantProperty *addProperty(int propertyType, const QString &name = QString());

    int propertyType(const QtProperty *property) const;
    int valueType(const QtProperty *property) const;
    QtVariantProperty *variantProperty(const QtProperty *property) const;

    virtual bool isPropertyTypeSupported(int propertyType) const;
    virtual int valueType(int propertyType) const;
    virtual QStringList attributes(int propertyType) const;
    virtual int attributeType(int propertyType, const QString &attribute) const;

    virtual QVariant value(const QtProperty *property) const;
    virtual QVariant attributeValue(const QtProperty *property, const QString &attribute) const;

   // the static functions below use for getting type id

    static int enumTypeId();               // QComboBox editor, for enum type
    static int iconMapTypeId();           // QtIconMap, use in enum type, set every icon associate with enum value,  QMap£ºenum value -> QIcon
    static int flagTypeId();                 // a group QCheckBox editor, value type is int, use bits as flags
    static int groupTypeId();              // represent a group properties, can just set a title for this group property
    static int groupWithTextTypeId();   // represent a group properties, can set a title for this group property
                                                      // and set a text value that associated with sub-properies

    static int pathTypeId();                // use for opening file, folder, or popup dialog and so on,
                                                    // provided a button and you need connect to signal openPathTriggered() for clicked event
    static int sliderDoubleTypeId();    // double value type of property,  and use slider to edit value

    static int customTypeId(); // custom type of property, can set value of QImage, QPixmap, EsColors, QRgb to property.
                                  // provided 2 button and you need connect to signal customOpenTriggered() for clicked event,
                                  // connect to signal customPopupTriggered() for popup event

    static int buttonTypeId(); // only a button for property. connect to signal buttonClicked()

    // 3 below does not implement now, just provide a button and emit signal openPathTriggered() when clicked
    static int gradientColorTypeId();   // gradient color
    static int colorTemplateTypeId();  // Color Template Type
    static int materialTypeId();           // material type
public Q_SLOTS:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setAttribute(QtProperty *property, const QString &attribute, const QVariant &value);
Q_SIGNALS:
    void valueChanged(QtProperty *property, const QVariant &val);
    void attributeChanged(QtProperty *property, const QString &attribute, const QVariant &val);

    // only use for property who's type is pathTypeId()
    void openPathTriggered(QtVariantProperty *property, QtVariantPropertyManager *thisManager = 0);

    // only use for property who's type is customTypeId()
    void customOpenTriggered(QtVariantProperty *property, QtVariantPropertyManager *thisManager = 0);

    // only use for property who's type is customTypeId()
    void customPopupTriggered(QtVariantProperty *property, QtVariantPropertyManager *thisManager = 0);

    // only use for property who's type is buttonTypeId()
    void buttonClicked(QtVariantProperty *property);
protected:
    virtual bool hasValue(const QtProperty *property) const;
    QString valueText(const QtProperty *property) const;
    QIcon valueIcon(const QtProperty *property) const;
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
    virtual QtProperty *createProperty();

    virtual bool hasCustomValue(const QtProperty *property) const;
    virtual QVariant customValue(const QtProperty *property) const;
private:
    class QtVariantPropertyManagerPrivate *d_ptr;
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, double, double))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotDecimalsChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, bool))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotRegExpChanged(QtProperty *, const QRegExp &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, const QDate &, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QTime &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QDateTime &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QKeySequence &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QChar &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QLocale &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QPoint &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QPointF &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QSize &))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, const QSize &, const QSize &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QSizeF &))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, const QSizeF &, const QSizeF &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QRect &))
    Q_PRIVATE_SLOT(d_func(), void slotConstraintChanged(QtProperty *, const QRect &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QRectF &))
    Q_PRIVATE_SLOT(d_func(), void slotConstraintChanged(QtProperty *, const QRectF &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QColor &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumNamesChanged(QtProperty *, const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumIconsChanged(QtProperty *, const QMap<int, QIcon> &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumValuesChanged(QtProperty *, const QList<int> &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QSizePolicy &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QFont &))
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty *, const QCursor &))
    Q_PRIVATE_SLOT(d_func(), void slotFlagNamesChanged(QtProperty *, const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void slotOpenPathTriggered(QtProperty *, QtPathPropertyManager *))
    Q_PRIVATE_SLOT(d_func(), void slotCustomOpenTriggered(QtProperty *, QtCustomPropertyManager *));
    Q_PRIVATE_SLOT(d_func(), void slotCustomPopupTriggered(QtProperty *, QtCustomPropertyManager *));
    Q_PRIVATE_SLOT(d_func(), void slotValueChanged(QtProperty*,QVariant))
    Q_PRIVATE_SLOT(d_func(), void slotButtonClicked(QtProperty*));

    Q_PRIVATE_SLOT(d_func(), void slotPropertyInserted(QtProperty *, QtProperty *, QtProperty *))
    Q_PRIVATE_SLOT(d_func(), void slotPropertyRemoved(QtProperty *, QtProperty *))
    Q_DECLARE_PRIVATE(QtVariantPropertyManager)
    Q_DISABLE_COPY(QtVariantPropertyManager)
};

class  QtVariantEditorFactory : public QtAbstractEditorFactory<QtVariantPropertyManager>
{
    Q_OBJECT
public:
    QtVariantEditorFactory(QObject *parent = 0);
    ~QtVariantEditorFactory();
protected:
    void connectPropertyManager(QtVariantPropertyManager *manager);
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtVariantPropertyManager *manager);
private:
    class QtVariantEditorFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtVariantEditorFactory)
    Q_DISABLE_COPY(QtVariantEditorFactory)
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

Q_DECLARE_METATYPE(QIcon)
Q_DECLARE_METATYPE(QtIconMap)
#endif
