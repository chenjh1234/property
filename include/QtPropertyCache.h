#ifndef QtVariantPropertyCache_h__
#define QtVariantPropertyCache_h__

#include <QScopedPointer>
#include <QVariant>
#include <QMap>

QT_BEGIN_NAMESPACE
class QtProperty;

class QtPropertyCache
{
public:
    typedef QMap<QtProperty*, QVariant> container;
    typedef container::const_iterator iterator;
    typedef container::const_iterator const_iterator;
public:
    QtPropertyCache() {}
    ~QtPropertyCache() {}

    void clear();

    // insert property object and corresponding value to the Cache, if property exists, update value
    void insert(QtProperty* property, const QVariant &value);

    // retrieve value corresponding to property, if property not exist, return an invalid QVariant.(see QVariant::isValid)
    QVariant getValue(QtProperty* property) const;

    // detect whether there is value corresponding to property in the Cache
    bool contains(QtProperty* property) const;

    bool isEmpty() const;

    QList<QtProperty*> properties() const;

    iterator begin() const;
    iterator end() const;
private:
    container mapPropertyToValue_;
};

//////////////////////////////////////////////////////////////////////////
// implementation

inline void QtPropertyCache::clear()
{
    mapPropertyToValue_.clear();
}

inline void QtPropertyCache::insert( QtProperty* property, const QVariant &value )
{
    mapPropertyToValue_[property] = value;
}

inline QVariant QtPropertyCache::getValue( QtProperty* property ) const
{
    return mapPropertyToValue_.value(property, QVariant());
}

inline bool QtPropertyCache::contains( QtProperty* property ) const
{
    return mapPropertyToValue_.contains(property);
}

inline bool QtPropertyCache::isEmpty() const
{
    return mapPropertyToValue_.isEmpty();
}

inline QList<QtProperty *> QtPropertyCache::properties() const
{
    return mapPropertyToValue_.keys();
}

inline QtPropertyCache::iterator QtPropertyCache::begin() const
{
    return mapPropertyToValue_.constBegin();
}

inline QtPropertyCache::iterator QtPropertyCache::end() const
{
    return mapPropertyToValue_.constEnd();
}

QT_END_NAMESPACE

#endif // QtVariantPropertyCache_h__
