#include "network.h"

Network::Network(QMainWindow *window, QString url) {
    this->window = window;
    request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    data = new QUrlQuery();
    manager = new QNetworkAccessManager;
}

void Network::add_data(QString key, QString value) {
    data->addQueryItem(key, value);
}
