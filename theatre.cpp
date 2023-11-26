#include "theatre.h"

theatre::theatre() {
    QString emp = "";
    name = emp;
    adress = emp;
    date = emp;
    head = emp;
}

theatre::theatre(QString n, QString a, QString d, QString h) {
    name = n;
    adress = a;
    date = d;
    head = h;
}

theatre::~theatre() {}
