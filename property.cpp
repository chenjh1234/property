#include <iostream>

using namespace std;
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"

int main (int argc, char *argv[])
{
    QtVariantPropertyManager m;
    QtVariantProperty *p;
    p = m.addProperty(QVariant::Int,"123");
    cout << "hello world" << endl;
    return(0);
}

