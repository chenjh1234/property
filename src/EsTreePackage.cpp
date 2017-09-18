#include "EsTreePackage.h"
#include <QCoreApplication>
#include <QTranslator>

EsTreePackage::EsTreePackage()
{
}

void EsTreePackage::initTranslation(QCoreApplication *app, const char *lang)
{
    if (strcmp(lang, "zh_cn") == 0) {
        QScopedPointer<QTranslator> translator(new QTranslator);
        if (translator->load(":/language/translations/EsTreePackage_zh_cn.qm")) {
            app->installTranslator(translator.data());
            translator.take();
        }
    }
}
