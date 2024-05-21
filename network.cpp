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
