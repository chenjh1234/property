#ifndef ESTREEPACKAGE_H
#define ESTREEPACKAGE_H

#include "estreepackage_global.h"

class QCoreApplication;

class ESTREEPACKAGESHARED_EXPORT EsTreePackage
{

public:
    EsTreePackage();

    static void initTranslation(QCoreApplication* app, char const* lang);
};

#endif // ESTREEPACKAGE_H
