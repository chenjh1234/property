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

    // ����Ľӿ����ڡ��Զ�����/�ֶ����¡����ܣ�Ĭ�����Զ����µģ��Ҳ�չʾ��Ӧ��UI�ؼ�������Ҫ���û����Ժ���
    // ����� QtPropertyCache
public:
    // ÿһҳ�����Ե������á��Ƿ��Զ����¡���Ĭ�����Զ����µġ�����ֻ�ṩ�����Ƿ���º͵��Ӧ�õ�UI����Ӧ���źţ�
    // ʵ�ʶ�����ֵ�Ļ����Ӧ����Ҫ�û�ʵ�֣�����ʹ�ñ����ṩ���� QtPropertyCache
    bool isAutoUpdateData(int tabIndex) const;
    void setAutoUpdateData(bool auto_, int tabIndex);

    // �Ƿ�չʾ���Զ�����/�ֶ����¡�UI�ؼ���Ĭ�ϲ�չʾ
    void setAutoUpdateUIVisible(bool visible, int tabIndex);
    bool isAutoUpdateUIVisible(int tabIndex) const;
Q_SIGNALS:
    // ������ֶ����£��û������Ӧ�á����ᴥ�����źš��û��󶨸��źſ��Ի��֪ͨȥ�����������ֵӦ�õ�ʵ�ʵĶ���
    void needApplyChangeToData(int tabIndex);

    // �û�����л����Զ�����/�ֶ����¡������ýӿ�setAutoUpdateData()���ò��ᴥ�����ź�
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
