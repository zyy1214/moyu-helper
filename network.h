#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMainWindow>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

class Network : public QObject
{
private:
    QMainWindow *window;
    QNetworkRequest *request;
    QUrlQuery *data;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

public:
    Network(QMainWindow *window, QString url);
    void add_data(QString key, QString value);

    void finished() {
        qDebug() << "aaa";
        reply->deleteLater();
    }

    template<class F1, class F2>
    void post(F1 f1, F2 f2) {
        reply = manager->post(*request, data->toString(QUrl::FullyEncoded).toUtf8());
        //reply = manager->get(*request);
        qDebug() << reply;
        qDebug() << reply->readAll();

        QObject::connect(reply, &QNetworkReply::finished, [&] () {
            if (reply->error() == QNetworkReply::NoError) {
                // 请求成功
                qDebug() << "Request succeeded";
                QString reply_string = reply->readAll();
                qDebug() << "Response:" << reply_string;
                f1(window, reply_string);
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
