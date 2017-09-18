#include "EsPropertyColorTemplate.h"
#include "qttreepropertybrowser.h"
#include "EsPropertyTabWidget.h"
#include "qtvariantproperty.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>

EsPropertyColorTemplate::EsPropertyColorTemplate(QString title, QList<QColor> *colors)
    : title(title)
    , colors(colors)
{
}

void EsPropertyColorTemplate::onUpdate()
{
    getVariantProperty("Number")->setValue(colors->count());
}

void EsPropertyColorTemplate::initializingProperty(EsPropertyTabWidget *editor, QtTreePropertyBrowser *browser, int indexBrowser)
{
    if (!useExistingBrowser())
        editor->setBrowserTitle(indexBrowser, title);

    QtVariantProperty* prop = 0;

    prop = propManager->addProperty(QtVariantPropertyManager::colorTemplateTypeId(), tr("Open template"));
    addProperty(prop, QLatin1String("Open template"), browser);

    prop = propManager->addProperty(QtVariantPropertyManager::colorTemplateTypeId(), tr("Save template"));
    addProperty(prop, QLatin1String("Save template"), browser);

    prop = propManager->addProperty(QVariant::Int, tr("Number"));
    prop->setAttribute(AttrName::minimum, 1);
    prop->setAttribute(AttrName::maximum, 1024);
    addProperty(prop, QLatin1String("Number"), browser);
    {
        QtVariantProperty* propColorNodes = propManager->addProperty(QtVariantPropertyManager::groupTypeId(), tr("Color setting"));

        for (int i = 0; i < colors->count(); i++) {
            QRgb color = colors->at(i).rgb();
            QString colorID = QString::number(i + 1);

            addOneColorProperty(color, colorID, propColorNodes);
        }

        addProperty(propColorNodes, QLatin1String("Color setting"), browser);
    }
    connect(propManager.data(), SIGNAL(openPathTriggered(QtVariantProperty*)),
            this, SLOT(openPathTriggered(QtVariantProperty*)), Qt::UniqueConnection);
}

void EsPropertyColorTemplate::onValueChanged(QtProperty *property, const QVariant &val)
{
    QString id = getId(property);
    if (id == QLatin1String("Number"))
    {
        int count = val.toInt();
        if (count > colors->count()) {
            for (int i = colors->count(); i < count; i++) {
                QRgb color = 0xFFFFFF;
                QString colorID = QString::number(i + 1);
                addOneColorProperty(color, colorID);
                colors->push_back(color);
            }
        }
        else if (count < colors->count()) {
            int prevCount = colors->count();
            for (int i = count; i < prevCount; i++) {
                QString colorID = QString::number(i + 1);
                removeProperty(colorID);
                colors->pop_back();
            }
        }
    }
    else
    {
        bool ok = false;
        int index = id.toInt(&ok) - 1;
        if (ok && val.canConvert<QRgb>()
            && index >= 0 && index < colors->count())
        {
            colors->replace(index, val.value<QRgb>());
        }
    }
}

void EsPropertyColorTemplate::addOneColorProperty(QRgb color, QString id, QtProperty *group)
{
    if (group == 0)
        group = getProperty("Color setting");
    removeProperty(id);
    QtVariantProperty* prop = propManager->addProperty(QtVariant::customTypeId(), id);
    prop->setValue(color);
    addProperty(prop, id, group);
}

void EsPropertyColorTemplate::openPathTriggered(QtVariantProperty *property)
{
    QString id = getId(property);
    if (id == "Open template")
    {
        QString filename = QFileDialog::getOpenFileName(
                    0, tr("Open template"), "", "color template (*.colortemp);;all file (*)");
        if (!filename.isEmpty())
        {
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly))
                QMessageBox::warning(0, tr("Error"), tr("Open file failed: ") + filename);
            else {
                QList<QColor> tmpColors;
                QDataStream ds(&file);
                ds >> tmpColors;
                resetTo(tmpColors);
            }
        }
    }
    else if (id == "Save template")
    {
        QString selectedFilter;
        QString filename = QFileDialog::getSaveFileName(
                    0, tr("Save template"), "", "color template (*.colortemp);;all file (*)", &selectedFilter);
        if (!filename.isEmpty())
        {
            if (selectedFilter.indexOf(".colortemp") >= 0 &&
                filename.lastIndexOf(".colortemp") < filename.length() - 10)
            {
                filename.append(".colortemp");
            }
            QFile file(filename);
            if (!file.open(QIODevice::WriteOnly))
                QMessageBox::warning(0, tr("Error"), tr("Save file failed: ") + filename);
            else {
                QDataStream ds(&file);
                ds << *colors;
            }
        }
    }
}

void EsPropertyColorTemplate::resetTo(const QList<QColor>& newColors)
{
    getVariantProperty("Number")->setValue(newColors.count());
    for (int i = 0; i < newColors.count(); i++)
    {
        if (QtVariantProperty* prop = getVariantProperty(QString::number(i + 1)))
            prop->setValue(newColors[i].rgb());
    }
}
