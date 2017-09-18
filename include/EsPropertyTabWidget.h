#ifndef ESPROPERTYTABWIDGET_H
#define ESPROPERTYTABWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
#include "estreepackage_global.h"

class QtTreePropertyBrowser;

class ESTREEPACKAGESHARED_EXPORT EsPropertyTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EsPropertyTabWidget(QWidget *parent = 0);
    ~EsPropertyTabWidget();

    void clear();
    int count() const;
    int currentIndex() const;
    void setCurrentIndex(int index);
    int indexOfBrowser(QtTreePropertyBrowser* browser) const;

    QtTreePropertyBrowser* currentBrowser() const;
    // Returns the browser at index position index or 0 if the index is out of range
    QtTreePropertyBrowser* getBrowser(int index = 0) const;

    // Adds a tab with the given browser to the tab widget, and returns the index of the added tab/browser.
    // The first tab index is 0. you should store the return value for later using,
    // e.g. used as parameter index when call other member function, except you have only one tab/browser
    int addBrowser(QtTreePropertyBrowser* browser, const QString& title = "");

    QString getBrowserTitle(int index = 0) const;
    void setBrowserTitle(int index, const QString& title);
    void setBrowserTitle(const QString& title) { setBrowserTitle(0, title); }

Q_SIGNALS:
    void browserCleared(QSet<QtTreePropertyBrowser*> const& browsers, EsPropertyTabWidget* editor = 0);
    void applyPropertyToDefault(int tabIndex);
    void getDefaultProperty(int tabIndex);

    // 下面的接口用于“自动更新/手动更新”功能，默认是自动更新的，且不展示对应的UI控件，不需要的用户可以忽略
    // 相关类 QtPropertyCache
public:
    // 每一页都可以单独设置“是否自动更新”，默认是自动更新的。本类只提供设置是否更新和点击应用的UI、对应的信号，
    // 实际对属性值的缓存和应用需要用户实现，可以使用本库提供的类 QtPropertyCache
    bool isAutoUpdateData(int tabIndex) const;
    void setAutoUpdateData(bool auto_, int tabIndex);

    // 是否展示“自动更新/手动更新”UI控件，默认不展示
    void setAutoUpdateUIVisible(bool visible, int tabIndex);
    bool isAutoUpdateUIVisible(int tabIndex) const;
Q_SIGNALS:
    // 如果是手动更新，用户点击“应用”，会触发该信号。用户绑定该信号可以获得通知去将缓存的属性值应用到实际的对象
    void needApplyChangeToData(int tabIndex);

    // 用户点击切换“自动更新/手动更新”，调用接口setAutoUpdateData()设置不会触发该信号
    void userChangedAutoUpdate(bool auto_);
private:
    QScopedPointer<class EsPropertyTabWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(EsPropertyTabWidget)
    Q_DISABLE_COPY(EsPropertyTabWidget)
    Q_PRIVATE_SLOT(d_func(), void on_checkBoxAutoUpdate_toggled(bool))
    Q_PRIVATE_SLOT(d_func(), void on_pushButtonApplyUpdate_clicked())
    Q_PRIVATE_SLOT(d_func(), void on_pushButtonSetDefault_clicked())
    Q_PRIVATE_SLOT(d_func(), void on_pushButtonGetDefault_clicked())
    Q_PRIVATE_SLOT(d_func(), void slotCurrentItemChanged(QtBrowserItem*))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentTabChanged(int))
};

QT_END_NAMESPACE
#endif // ESPROPERTYTABWIDGET_H
