#include "EsPropertyTabWidget.h"
#include "ui_EsPropertyTabWidget.h"
#include <QtGui>
#include "qttreepropertybrowser.h"

class MTabWidget : public QTabWidget
{
public:
    explicit MTabWidget(QWidget* parent = 0) : QTabWidget(parent) { }
    QTabBar* tabBar() const { return QTabWidget::tabBar(); }
};

class EsPropertyTabWidgetPrivate
{
public:
    EsPropertyTabWidgetPrivate(EsPropertyTabWidget* editor);

    void checkTabBarVisible();
    void checkBeforeAddTabPage();

    QtTreePropertyBrowser* getAloneBrowser() const;

    int d_insertBrowser(int index, QtTreePropertyBrowser* browser, const QString& title);

    void d_setAutoUpdateUIVisible(bool visible);
    void d_setAutoUpdateUICheched(bool checked);
private: // slots
    void on_checkBoxAutoUpdate_toggled(bool checked);
    void on_pushButtonApplyUpdate_clicked();
    void on_pushButtonSetDefault_clicked();
    void on_pushButtonGetDefault_clicked();

    void slotCurrentTabChanged(int index);
    void slotCurrentItemChanged(QtBrowserItem* current);
private:
    Q_DECLARE_PUBLIC(EsPropertyTabWidget)
    EsPropertyTabWidget* q_ptr;
    Ui::EsPropertyTabWidget ui;

    MTabWidget tabWidget_;
    QString firstBrowserTitle_;
    quint8 hasBrowserAlone_;

    QVector<quint8> autoUpdate_;        // 标记每页是否自动更新
    QVector<quint8> updateUIVisible_;    // 标记每页的更新UI控件是否展示
    QLayoutItem* layoutItemUpdateUI_;    // 更新UI控件对应的QLayoutItem
};

EsPropertyTabWidgetPrivate::EsPropertyTabWidgetPrivate( EsPropertyTabWidget* editor )
    : q_ptr(editor)
    , tabWidget_(editor)
    , hasBrowserAlone_(0)
    , layoutItemUpdateUI_(0)
{
    ui.setupUi(editor);
    d_setAutoUpdateUIVisible(false);
    ui.verticalLayout->insertWidget(0, &tabWidget_);
    checkTabBarVisible();
    slotCurrentTabChanged(-1);
    QObject::connect(&tabWidget_, SIGNAL(currentChanged(int)), q_ptr, SLOT(slotCurrentTabChanged(int)));
}

void EsPropertyTabWidgetPrivate::checkTabBarVisible()
{
    Q_Q(EsPropertyTabWidget);
    int c = tabWidget_.count();
    if (c <= 1) {
        tabWidget_.hide();
        if (c == 1) {
            if (QtTreePropertyBrowser* brower = q->getBrowser()) {
                hasBrowserAlone_ = 1;
                firstBrowserTitle_ = tabWidget_.tabText(0);
                tabWidget_.removeTab(0);
                brower->setParent(q);
                brower->show();
                ui.verticalLayout->insertWidget(0, brower);
            }
        }
    }
    else if (c == 2) {
        tabWidget_.show();
    }
    ui.labelStatus->setText("");
}

void EsPropertyTabWidgetPrivate::checkBeforeAddTabPage()
{
    if (hasBrowserAlone_) {
        hasBrowserAlone_ = 0;
        QLayoutItem* item = ui.verticalLayout->takeAt(0);
        if (QtTreePropertyBrowser* brower = qobject_cast<QtTreePropertyBrowser*>(item->widget())) {
            brower->setParent(&tabWidget_);
            tabWidget_.insertTab(0, brower, firstBrowserTitle_);
            tabWidget_.show();
            firstBrowserTitle_ = "";
        }
        delete item;
    }
}

QtTreePropertyBrowser* EsPropertyTabWidgetPrivate::getAloneBrowser() const
{
    if (QLayoutItem* item = ui.verticalLayout->itemAt(0))
        if (QWidget* widget = item->widget())
            return qobject_cast<QtTreePropertyBrowser*>(widget);
    return 0;
}

void EsPropertyTabWidgetPrivate::slotCurrentItemChanged( QtBrowserItem* current )
{
    QString tip;
    if (current)
        if (QtProperty* property = current->property())
            tip = property->statusTip();
    ui.labelStatus->setText(tip);
}

/*

    implementation of public class EsPropertyTabWidget
*/

EsPropertyTabWidget::EsPropertyTabWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new EsPropertyTabWidgetPrivate(this))
{
}

EsPropertyTabWidget::~EsPropertyTabWidget()
{

}

void EsPropertyTabWidget::clear()
{
    Q_D(EsPropertyTabWidget);
    bool prevBlocked = d->tabWidget_.blockSignals(true);
    d->checkBeforeAddTabPage();

    if (receivers(SIGNAL(needApplyChangeToData(int))) > 0) {
        for (int i = 0; i < d->autoUpdate_.count(); i++)
            if (!bool(d->autoUpdate_[i]))
                emit needApplyChangeToData(i);
    }

    QSet<QtTreePropertyBrowser*> browsers;
    bool needEmitCleared = false;
    if (receivers(SIGNAL(browserCleared(QSet<QtTreePropertyBrowser*> const&, EsPropertyTabWidget*))) > 0) {
        needEmitCleared = true;
        for (int i = 0; i < d->tabWidget_.count(); i++) {
            QWidget* w = d->tabWidget_.widget(i);
            if (QtTreePropertyBrowser* tree = qobject_cast<QtTreePropertyBrowser*>(w))
                browsers.insert(tree);
        }
    }
    d->tabWidget_.clear();
    d->autoUpdate_.clear();
    d->updateUIVisible_.clear();
    d->checkTabBarVisible();
    d->slotCurrentTabChanged(-1);

    d->tabWidget_.blockSignals(prevBlocked);
    if (needEmitCleared)
        emit browserCleared(browsers, this);
}

int EsPropertyTabWidget::count() const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_)
        return 1;
    else
        return d->tabWidget_.count();
}

int EsPropertyTabWidget::currentIndex() const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_)
        return 0;
    else
        return d->tabWidget_.currentIndex();
}

void EsPropertyTabWidget::setCurrentIndex(int index)
{
    Q_D(EsPropertyTabWidget);
    if (!d->hasBrowserAlone_)
        d->tabWidget_.setCurrentIndex(index);
}

int EsPropertyTabWidget::indexOfBrowser(QtTreePropertyBrowser *browser) const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_)
        return (browser == d->getAloneBrowser()) ? 0 : -1;
    else
        return d->tabWidget_.indexOf(browser);
}

QString EsPropertyTabWidget::getBrowserTitle( int index /*= 0*/ ) const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_) {
        if (index == 0)
            return d->firstBrowserTitle_;
        else
            return "";
    }
    else {
        if (index < 0 || index >= count())
            return "";
        else
            return d->tabWidget_.tabText(index);
    }
}

void EsPropertyTabWidget::setBrowserTitle( int index, const QString& title )
{
    Q_D(EsPropertyTabWidget);
    if (d->hasBrowserAlone_) {
        if (index == 0)
            d->firstBrowserTitle_ = title;
    }
    else {
        if (index >= 0 && index < count())
            d->tabWidget_.setTabText(index, title);
    }
}

QtTreePropertyBrowser* EsPropertyTabWidget::currentBrowser() const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_) {
        return d->getAloneBrowser();
    }
    else {
        QWidget* widget = d->tabWidget_.currentWidget();
        return qobject_cast<QtTreePropertyBrowser*>(widget);
    }
}

QtTreePropertyBrowser* EsPropertyTabWidget::getBrowser( int index /*= 0*/ ) const
{
    Q_D(const EsPropertyTabWidget);
    if (d->hasBrowserAlone_) {
        if (index == 0)
            return d->getAloneBrowser();
        else
            return 0;
    }
    else {
        if (QWidget* widget = d->tabWidget_.widget(index))
            return qobject_cast<QtTreePropertyBrowser*>(widget);
        return 0;
    }
}

int EsPropertyTabWidget::addBrowser( QtTreePropertyBrowser* browser, const QString& title /*= ""*/ )
{
    Q_D(EsPropertyTabWidget);
    return d->d_insertBrowser(count(), browser, title.isEmpty() ? "browser" : title);
}

int EsPropertyTabWidgetPrivate::d_insertBrowser( int index, QtTreePropertyBrowser* browser, const QString& title )
{
    if (!browser) return -1;

    Q_Q(EsPropertyTabWidget);
    bool prevBlocked = tabWidget_.blockSignals(true);
    int prevIndex = q->currentIndex();
    checkBeforeAddTabPage();

    int result = index = tabWidget_.insertTab(index, browser, title);
    autoUpdate_.insert(index, 1);
    updateUIVisible_.insert(index, 0);

    QObject::connect(browser, SIGNAL(currentItemChanged(QtBrowserItem*)), q, SLOT(slotCurrentItemChanged(QtBrowserItem*)));
    checkTabBarVisible();
    if (prevIndex != q->currentIndex())
        slotCurrentTabChanged(q->currentIndex());
    tabWidget_.blockSignals(prevBlocked);
    return result;
}

//{begin 手动更新
// 下面的接口用于“自动更新/手动更新”功能，默认是自动更新的，且不展示对应的UI控件，不需要的用户可以忽略
#if 1 

void EsPropertyTabWidgetPrivate::on_pushButtonApplyUpdate_clicked()
{
    Q_Q(EsPropertyTabWidget);
    int index = q->currentIndex();
    if (index >= 0)
        emit q->needApplyChangeToData(index);
}

void EsPropertyTabWidgetPrivate::on_pushButtonSetDefault_clicked()
{
    Q_Q(EsPropertyTabWidget);
    ui.pushButtonSetDefault->setEnabled(false);
    emit q->applyPropertyToDefault(q->currentIndex());
    ui.pushButtonSetDefault->setEnabled(true);
}

void EsPropertyTabWidgetPrivate::on_pushButtonGetDefault_clicked()
{
    Q_Q(EsPropertyTabWidget);
    ui.pushButtonGetDefault->setEnabled(false);
    emit q->getDefaultProperty(q->currentIndex());
    ui.pushButtonGetDefault->setEnabled(true);
}

void EsPropertyTabWidgetPrivate::d_setAutoUpdateUIVisible( bool visible )
{
    ui.checkBoxAutoUpdate->setVisible(visible);
    ui.pushButtonApplyUpdate->setVisible(visible);
    if (visible) {
        if (layoutItemUpdateUI_)
            ui.verticalLayout->addItem(layoutItemUpdateUI_);
    }
    else
        layoutItemUpdateUI_ = ui.verticalLayout->takeAt(ui.verticalLayout->count() - 1);
}

void EsPropertyTabWidgetPrivate::d_setAutoUpdateUICheched( bool checked )
{
    bool prevBlocked = ui.checkBoxAutoUpdate->blockSignals(true);
    ui.checkBoxAutoUpdate->setChecked(checked);
    ui.pushButtonApplyUpdate->setEnabled(!checked);
    ui.checkBoxAutoUpdate->blockSignals(prevBlocked);
}

void EsPropertyTabWidgetPrivate::on_checkBoxAutoUpdate_toggled( bool checked )
{
    Q_Q(EsPropertyTabWidget);
    int index = q->currentIndex();
    if (index >= 0) {
        autoUpdate_[index] = checked;
        ui.pushButtonApplyUpdate->setEnabled(!checked);
        emit q->userChangedAutoUpdate(checked);
    }
}

void EsPropertyTabWidgetPrivate::slotCurrentTabChanged( int index )
{
    bool show = false;
    if (index >= 0 && index < updateUIVisible_.count())
        show = updateUIVisible_[index];
    if (show != ui.checkBoxAutoUpdate->isVisible())
        d_setAutoUpdateUIVisible(show);
    if (show)
        d_setAutoUpdateUICheched(autoUpdate_[index]);

    bool showDefaultButton = (index >= 0);
    ui.pushButtonSetDefault->setVisible(showDefaultButton);
    ui.pushButtonGetDefault->setVisible(showDefaultButton);
    ui.line->setVisible(showDefaultButton);
}

bool EsPropertyTabWidget::isAutoUpdateData( int index) const
{
    Q_D(const EsPropertyTabWidget);
    if (index >= 0 && index < count())
        return d->autoUpdate_[index];
    return false;

}

void EsPropertyTabWidget::setAutoUpdateData( bool auto_, int index )
{
    Q_D(EsPropertyTabWidget);
    if (index >= 0 && index < count() && (bool)d->autoUpdate_[index] != auto_)
    {
        d->autoUpdate_[index] = auto_;
        if (currentIndex() == index)
            d->d_setAutoUpdateUICheched(auto_);
    }
}

void EsPropertyTabWidget::setAutoUpdateUIVisible( bool visible, int index )
{
    Q_D(EsPropertyTabWidget);
    if (index >= 0 && index < count() && (bool)d->updateUIVisible_[index] != visible)
    {
        d->updateUIVisible_[index] = visible;
        if (currentIndex() == index)
            d->d_setAutoUpdateUIVisible(visible);
        if (visible)
            d->d_setAutoUpdateUICheched(d->autoUpdate_[index]);
    }
}

bool EsPropertyTabWidget::isAutoUpdateUIVisible(int index) const
{
    Q_D(const EsPropertyTabWidget);
    if (index >= 0 && index < count())
        return d->updateUIVisible_[index];
    return false;
}

#endif //}end. 手动更新

#include "moc_EsPropertyTabWidget.cpp"
