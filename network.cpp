#include "network.h"

#include <QTimer>

Network::Network(QString url) {
    request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    query_data = new QUrlQuery();
    manager = new QNetworkAccessManager;
}

void Network::add_data(QString key, QString value) {
    query_data->addQueryItem(key, value);
}

void Network::post(int timeout) {
    reply = manager->post(*request, query_data->toString(QUrl::FullyEncoded).toUtf8());
    qDebug() << reply;
    qDebug() << reply->readAll();

    if (timeout > 0) {
        QTimer *timeoutTimer = new QTimer();
        timeoutTimer->setSingleShot(true);
        QObject::connect(reply, &QNetworkReply::finished, timeoutTimer, &QTimer::stop);
        QObject::connect(timeoutTimer, &QTimer::timeout, [=] () {
            qDebug() << "timeout";
            reply->abort();
        });
        timeoutTimer->start(timeout);
    }

    QObject::connect(reply, &QNetworkReply::finished, [=] () {
        if (reply->error() == QNetworkReply::NoError) {
            // 请求成功
            qDebug() << "Request succeeded";
            QString reply_string = reply->readAll();
            qDebug() << "Response:" << reply_string;
            emit succeeded(reply_string);
        } else {
            qDebug() << "Error:" << reply->errorString();
            emit failed();
        }
        reply->deleteLater();
    });
}

void Network::get() {
    reply = manager->get(*request, query_data->toString(QUrl::FullyEncoded).toUtf8());
    qDebug() << reply;
    qDebug() << reply->readAll();

    QObject::connect(reply, &QNetworkReply::finished, [&] () {
        if (reply->error() == QNetworkReply::NoError) {
            // 请求成功
            qDebug() << "Request succeeded";
            QString reply_string = reply->readAll();
            qDebug() << "Response:" << reply_string;
            emit succeeded(reply_string);
            reply->deleteLater();
        } else {
            qDebug() << "Error:" << reply->errorString();
            emit failed();
        }
        reply->deleteLater();
    });
}

void Network::finished() {
    reply->deleteLater();
}
