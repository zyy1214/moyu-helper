#include "network.h"

Network::Network(QMainWindow *window, QString url) {
    this->window = window;
    this->data = nullptr;
    request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    query_data = new QUrlQuery();
    manager = new QNetworkAccessManager;
}
Network::Network(Data *data, QString url) {
    this->data = data;
    this->window = nullptr;
    request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    query_data = new QUrlQuery();
    manager = new QNetworkAccessManager;
}

void Network::add_data(QString key, QString value) {
    query_data->addQueryItem(key, value);
}

void Network::post() {
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
            emit succeeded(reply_string);
            reply->deleteLater();
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
