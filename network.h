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
private:
    QMainWindow *window;
    QNetworkRequest *request;
    QUrlQuery *query_data;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    Data *data;

public:
    Network(QMainWindow *window, QString url);
    Network(Data *data, QString url);
    void add_data(QString key, QString value);

    void finished() {
        qDebug() << "aaa";
        reply->deleteLater();
    }

    template<class F1, class F2>
    void post(F1 f1, F2 f2) {
        reply = manager->post(*request, query_data->toString(QUrl::FullyEncoded).toUtf8());
        //reply = manager->get(*request);
        qDebug() << reply;
        qDebug() << reply->readAll();

        QObject::connect(reply, &QNetworkReply::finished, [&] () {
            if (reply->error() == QNetworkReply::NoError) {
                // 请求成功
                qDebug() << "Request succeeded";
                QString reply_string = reply->readAll();
                qDebug() << "Response:" << reply_string;
                if (window) f1(window, reply_string);
                else f1(data, reply_string);
                reply->deleteLater();
            } else {
                qDebug() << "Error:" << reply->errorString();
                f2(window);
            }
            reply->deleteLater();
        });
    }

    template<class F1, class F2>
    void get(F1 f1, F2 f2) {
        reply = manager->get(*request, query_data->toString(QUrl::FullyEncoded).toUtf8());
        qDebug() << reply;
        qDebug() << reply->readAll();

        QObject::connect(reply, &QNetworkReply::finished, [&] () {
            if (reply->error() == QNetworkReply::NoError) {
                // 请求成功
                qDebug() << "Request succeeded";
                QString reply_string = reply->readAll();
                qDebug() << "Response:" << reply_string;
                if (window) f1(window, reply_string);
                else f1(data, reply_string);
                reply->deleteLater();
            } else {
                qDebug() << "Error:" << reply->errorString();
                f2(window);
            }
            reply->deleteLater();
        });
    }

// private slots:
//     void reply_finished() {
//         reply->deleteLater();
//     }
};

#endif // NETWORK_H
