#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMainWindow>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

#include "data_storage.h"

class Network : public QObject
{
    Q_OBJECT
private:
    QNetworkRequest *request;
    QUrlQuery *query_data;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

public:
    Network(QString url);
    void add_data(QString key, QString value);

    void finished();
    void post();
    void get();

signals:
    void succeeded(QString reply_string);
    void failed();
};

#endif // NETWORK_H
