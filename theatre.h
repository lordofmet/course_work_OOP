#pragma once
#ifndef THEATRE_H
#define THEATRE_H

#include <QString>

class theatre {
public:
    QString name;
    QString adress;
    QString date;
    QString head;

    theatre();
    theatre(QString n, QString a, QString d, QString h);
    ~theatre();
};

#endif // THEATRE_H
