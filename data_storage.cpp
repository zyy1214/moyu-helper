#include <QCloseEvent>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "record.h"
#include "data_storage.h"
#include "network.h"

bool isTableExists(const QString& tableName, QSqlDatabase& db) {
    QSqlQuery query(db);
    query.prepare("SELECT count(*) FROM sqlite_master WHERE type='table' AND name=:tableName");
    query.bindValue(":tableName", tableName);

    if (!query.exec()) {
        qDebug() << "Error:" << query.lastError().text();
        return false;
    }

    query.next();
    int count = query.value(0).toInt();
    return (count > 0);
}

bool db_kv_inited = false;
QSqlDatabase db_kv;
std::unordered_map<QString, QString> kv_cache;

void init_db_kv() {
    if (!db_kv_inited) {
        db_kv = QSqlDatabase::addDatabase("QSQLITE");
        db_kv.setDatabaseName("database.db");
        if (!db_kv.open()) {
            qDebug() << "Error: Failed to connect to database:" << db_kv.lastError().text();
            return;
        } else {
            qDebug() << "Connected to db.";
        }
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS kv_table ("
                   "key VARCHAR(255) PRIMARY KEY, "
                   "value TEXT"
                   ")");
        query.exec("CREATE INDEX IF NOT EXISTS key_index ON kv_table (key)");
        db_kv_inited = true;
    }
}
void save_value(QString key, QString value, bool user_specific) {
    init_db_kv();
    if (user_specific) {
        key += "__" + get_value("userid");
    }
    QSqlQuery query(db_kv);
    query.prepare("SELECT COUNT(*) FROM kv_table WHERE key = :key");
    query.bindValue(":key", key);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }
    query.next();
    int count = query.value(0).toInt();
    if (count > 0) {
        query.prepare("UPDATE kv_table SET value = :value WHERE key = :key");
    } else {
        query.prepare("INSERT INTO kv_table (key, value) VALUES (:key, :value)");
    }
    query.bindValue(":key", key);
    query.bindValue(":value", value);
    if (!query.exec()) {
        qDebug() << "Failed to insert/update key-value pair:" << query.lastError().text();
    }
    kv_cache[key] = value;
}
QString get_value(QString key, bool user_specific, QString default_value) {
    init_db_kv();
    if (user_specific) {
        key += "__" + get_value("userid");
    }
    if (kv_cache.find(key) != kv_cache.end()) {
        return kv_cache[key];
    }
    QSqlQuery query(db_kv);
    query.prepare("SELECT value FROM kv_table WHERE key = :key");
    query.bindValue(":key", key);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return default_value;
    }
    if (query.next()) {
        QString retrievedValue = query.value(0).toString();
        //qDebug() << "Retrieved value for key" << key << ":" << retrievedValue;
        kv_cache[key] = retrievedValue;
        return retrievedValue;
    } else {
        qDebug() << "Key" << key << "not found in the database.";
        kv_cache[key] = default_value;
    }
    return default_value;
}

QSqlDatabase db;

QString username;
void load_data(Data *data) {
    username = get_value("username");
    username = username == "" ? "local" : "user-" + username;
    db = QSqlDatabase::addDatabase("QSQLITE", "main");
    db.setDatabaseName("user_" + get_value("userid") + ".db");

    if (!db.open()) {
        qDebug() << "Error: Failed to connect to database:" << db.lastError().text();
        return;
    } else {
        qDebug() << "Connected to db.";
    }

    QSqlQuery query_create(db);

    // 创建表
    QString create_records_table_query = "CREATE TABLE IF NOT EXISTS records ("
                                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "uuid TEXT,"
                                         "user TEXT,"
                                         "create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                                         "date DATE,"
                                         "sort_order INTEGER,"
                                         "record_class INTEGER,"
                                         "record_type INTEGER,"
                                         "point INTEGER,"
                                         "record_info TEXT)";

    if (!query_create.exec(create_records_table_query)) {
        qDebug() << "Error creating table records:" << query_create.lastError().text();
        return;
    }
    query_create.exec("CREATE INDEX IF NOT EXISTS user_index ON records (user)");

    QString create_mods_table_query = "CREATE TABLE IF NOT EXISTS mods ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "uuid TEXT,"
                                      "user TEXT,"
                                      "create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                                      "mod_type INTEGER,"
                                      "deleted INTEGER,"
                                      "name TEXT,"
                                      "content TEXT,"
                                      "name_merge TEXT,"
                                      "formula TEXT,"
                                      "labels TEXT)";

    if (!query_create.exec(create_mods_table_query)) {
        qDebug() << "Error creating table mods:" << query_create.lastError().text();
        return;
    }
    query_create.exec("CREATE INDEX IF NOT EXISTS user_index ON mods (user)");


    QString create_mod_operations_query = "CREATE TABLE IF NOT EXISTS mod_operations ("
                                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                          "uuid TEXT,"
                                          "user TEXT,"
                                          "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                                          "operation_type INT,"
                                          "mod_uuid TEXT,"
                                          "mod_type INTEGER,"
                                          "deleted INTEGER,"
                                          "name TEXT,"
                                          "content TEXT,"
                                          "name_merge TEXT,"
                                          "formula TEXT,"
                                          "labels TEXT)";
    if (!query_create.exec(create_mod_operations_query)) {
        qDebug() << "Error creating table mod_operations:" << query_create.lastError().text();
        return;
    }
    query_create.exec("CREATE INDEX IF NOT EXISTS user_index ON mod_operations (user)");


    QString create_record_operations_query = "CREATE TABLE IF NOT EXISTS record_operations ("
                                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                             "uuid TEXT,"
                                             "user TEXT,"
                                             "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                                             "operation_type INT,"
                                             "record_uuid TEXT,"
                                             "record_type INTEGER,"
                                             "record_class INTEGER,"
                                             "point INTEGER,"
                                             "record_info TEXT,"
                                             "record_date DATE,"
                                             "sort_order INTEGER)";
    if (!query_create.exec(create_record_operations_query)) {
        qDebug() << "Error creating table record_operations:" << query_create.lastError().text();
        return;
    }
    query_create.exec("CREATE INDEX IF NOT EXISTS user_index ON record_operations (user)");


    QSqlQuery query(db);


    // 准备查询语句
    QString mods_query = "SELECT * FROM mods WHERE user = :username "
                         "ORDER BY create_time ASC";

    query.prepare(mods_query);
    query.bindValue(":username", username);
    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error selecting mods:" << query.lastError().text();
        return;
    }

    std::unordered_map<QString, Mod *> uuid_map;
    while (query.next()) {
        int id = query.value("id").toInt();
        //QDateTime createTime = query.value("create_time").toDateTime();
        enum RECORD_TYPE mod_type = query.value("mod_type").toInt() == 1 ? OBTAIN : CONSUME;
        bool deleted = query.value("deleted").toInt();
        QString name = query.value("name").toString();
        QString content = query.value("content").toString();
        QString name_merge = query.value("name_merge").toString();
        QString formula = query.value("formula").toString();
        QString labels = query.value("labels").toString();
        QString uuid = query.value("uuid").toString();

        Mod *mod = new Mod(id, content, new Formula(formula), mod_type, name);
        mod->set_deleted(deleted);
        mod->set_name_merge(name_merge);
        mod->set_labels_string(labels);
        mod->set_uuid(uuid);
        data->mods.push_back(mod);

        uuid_map[uuid] = mod;
    }

    // 准备查询语句
    QString records_query = "SELECT * FROM records WHERE user = :username "
                          "ORDER BY date DESC, sort_order DESC, create_time DESC";

    query.prepare(records_query);
    query.bindValue(":username", username);
    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error selecting records:" << query.lastError().text();
        return;
    }

    QDate *last = nullptr;
    MultipleRecord *mr = nullptr;
    while (query.next()) {
        int id = query.value("id").toInt();
        //QDateTime createTime = query.value("create_time").toDateTime();
        QDate date = query.value("date").toDate();
        enum RECORD_CLASS record_class = query.value("record_class").toInt() == 1 ? BY_MOD : DIRECT;
        enum RECORD_TYPE record_type = query.value("record_type").toInt() == 1 ? OBTAIN : CONSUME;
        int point = query.value("point").toInt();
        QString record_info = query.value("record_info").toString();
        QString uuid = query.value("uuid").toString();
        int sort_order = query.value("sort_order").toInt();

        if (!last || date != (*last)) {
            if (mr) data->records[*last] = mr;
            last = new QDate(date);
            mr = new MultipleRecord;
        }

        Record *record = nullptr;
        switch (record_class) {
            case BY_MOD: {
                record = new RecordByMod(nullptr, nullptr, date);
                record->from_string(uuid_map, record_info);
                break;
            }
            case DIRECT: {
                record = new RecordDirect(record_info, record_type, point, date);
                break;
            }
        }
        if (record) {
            record->set_id(id);
            record->set_uuid(uuid);
            record->set_sort_order(sort_order);
            mr->push_back(record);
        } else {
            qDebug() << "error: record is not initialized!";
        }
    }
    if (mr) {
        data->records[*last] = mr;
    }

    sync_data(data);
}

class LoadingDialog : public QDialog {
    //Q_OBJECT

public:
    LoadingDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("同步数据中");

        Qt::WindowFlags flags = windowFlags();
        flags &= ~Qt::WindowTitleHint;
        flags |= Qt::CustomizeWindowHint;
        setWindowFlags(flags);

        setPalette(QPalette(QColor(Qt::white)));

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(30, 20, 0, 20);
        QLabel *label = new QLabel("同步数据中，请稍后……", this);
        label->setStyleSheet("font-size: 16px;");
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setFixedWidth(300);

        progressBar->setRange(0, 0);

        layout->addWidget(progressBar);
        layout->addSpacing(15);
        layout->addWidget(label);

        setLayout(layout);
        //setFixedSize(250, 300); // 固定窗口大小
    }
protected:
    void closeEvent(QCloseEvent *event) override {
        event->ignore(); // 忽略关闭事件
    }
};
LoadingDialog *loadingDialog;
QTimer *timer;
void finish_load() {
    try {
        if (loadingDialog) {
            loadingDialog->close();
            delete loadingDialog;
            loadingDialog = nullptr;
        }
        if (timer) {
            timer->stop();
        }
    }
    catch (std::exception e) {
        qDebug() << e.what();
    }
}


void Data::sync_records() {
    sync_records_data(this);
    QObject::disconnect(this, &Data::mod_sync_finished, this, &Data::sync_records);
}
void sync_data(Data *data) {
    sync_mods_data(data);
    QObject::connect(data, &Data::mod_sync_finished, data, &Data::sync_records);

    //QThread workerThread;
    timer = new QTimer();

    //workerThread.start();
    timer->start(300);

    QObject::connect(timer, &QTimer::timeout, [=] () {
        loadingDialog = new LoadingDialog();
        loadingDialog->exec();
    });

}

enum RECORD_OPERATION_TYPE {
    ADD_RECORD = 1, MODIFY_RECORD = 2, DELETE_RECORD = 3
};

// 合并来自网络的 record 操作记录
void merge_record_operations(Data *data, QJsonArray &jsonArray) {
    QSqlQuery query(db);

    QString earliest_time = "9999-99-99 99:99:99";
    // 将获取到的操作记录存入数据库中
    for (const QJsonValue &value : jsonArray) {
        QJsonObject operation = value.toObject();
        query.prepare("SELECT id FROM record_operations WHERE uuid = :uuid");
        query.bindValue(":uuid", operation["uuid"].toString());
        query.exec();
        int cnt = 0;
        while (query.next()) {
            cnt++;
        }
        if (cnt != 0) continue;

        query.prepare("INSERT INTO record_operations (uuid, user, time, operation_type, record_uuid, "
                      "record_type, record_class, point, record_info, record_date, sort_order) "
                      "VALUES (:uuid, :username, :time, :operation_type, :record_uuid, "
                      ":record_type, :record_class, :point, :record_info, :record_date, :sort_order)");
        query.bindValue(":uuid", operation["uuid"].toString());
        query.bindValue(":username", username);
        QString time = operation["time"].toString();
        qDebug() << time;
        if (time < earliest_time) earliest_time = time;
        query.bindValue(":time", time);
        query.bindValue(":operation_type", operation["operation_type"].toInt());
        query.bindValue(":record_uuid", operation["record_uuid"].toString());
        query.bindValue(":record_type", operation["record_type"].toInt());
        query.bindValue(":record_class", operation["record_class"].toInt());
        query.bindValue(":point", operation["point"].toInt());
        query.bindValue(":record_info", operation["record_info"].toString());
        query.bindValue(":record_date", operation["record_date"].toString());
        query.bindValue(":sort_order", operation["sort_order"].toInt());
        if (!query.exec()) {
            qDebug() << "Failed to insert operation:" << query.lastError().text();
        }
    }

    std::unordered_map<QString, Record *> uuid_map;
    for (auto mr : data->records) {
        for (Record *record : *(mr.second)) {
            uuid_map[record->get_uuid().toString(QUuid::WithoutBraces)] = record;
        }
    }
    std::unordered_map<QString, Mod *> mod_uuid_map;
    for (Mod *mod : data->mods) {
        mod_uuid_map[mod->get_uuid().toString(QUuid::WithoutBraces)] = mod;
    }


    qDebug() << earliest_time;
    query.prepare("SELECT * FROM record_operations WHERE time >= :earliest_time ORDER BY time ASC");
    query.bindValue(":earliest_time", earliest_time);
    if (!query.exec()) {
        qDebug() << "Failed to get record_operations:" << query.lastError().text();
    }
    while (query.next()) {
        enum RECORD_OPERATION_TYPE operation_type;
        switch (query.value("operation_type").toInt()) {
            case 1: operation_type = ADD_RECORD; break;
            case 2: operation_type = MODIFY_RECORD; break;
            case 3: operation_type = DELETE_RECORD; break;
        }

        QString record_info = query.value("record_info").toString();
        QDate record_date = query.value("record_date").toDate();
        int sort_order = query.value("sort_order").toInt();
        QString record_uuid = query.value("record_uuid").toString();
        enum RECORD_TYPE record_type = query.value("record_type").toInt() == 1 ? OBTAIN : CONSUME;
        enum RECORD_CLASS record_class = query.value("record_class").toInt() == 1 ? BY_MOD : DIRECT;
        int point = query.value("point").toInt();

        switch (operation_type) {
            case ADD_RECORD: {
                if (uuid_map.find(record_uuid) == uuid_map.end()) {
                    Record *record = nullptr;
                    switch (record_class) {
                        case BY_MOD: {
                            record = new RecordByMod(nullptr, nullptr, record_date);
                            record->from_string(mod_uuid_map, record_info);
                            break;
                        }
                        case DIRECT: {
                            record = new RecordDirect(record_info, record_type, point, record_date);
                            break;
                        }
                    }
                    record->set_uuid(record_uuid);
                    record->set_sort_order(sort_order);
                    record->set_id(db_add_record(data, record, false));
                    uuid_map[record_uuid] = record;
                    if (data->records.find(record_date) == data->records.end()) {
                        data->records[record_date] = new MultipleRecord;
                    }
                    data->records[record_date]->add_record(record);
                    // emit data->record_added(record);
                }
                break;
            }
            case MODIFY_RECORD: {
                if (uuid_map.find(record_uuid) != uuid_map.end()) {
                    switch (record_class) {
                        case BY_MOD: {
                            RecordByMod *record = dynamic_cast<RecordByMod *>(uuid_map[record_uuid]);
                            if (!record) {
                                qDebug() << "Error! record_class don't match.";
                                continue;
                            }
                            record->from_string(mod_uuid_map, record_info);
                            break;
                        }
                        case DIRECT: {
                            RecordDirect *record = dynamic_cast<RecordDirect *>(uuid_map[record_uuid]);
                            if (!record) {
                                qDebug() << "Error! record_class don't match.";
                                continue;
                            }
                            record->set_name(record_info);
                            record->set_point(point);
                            record->set_type(record_type);
                            break;
                        }
                    }
                    Record *record = uuid_map[record_uuid];
                    record->set_sort_order(sort_order);
                    record->set_date(record_date);
                    // emit data->record_modified(record);
                    db_modify_record(data, record, false);
                } else {
                    // 可能是已经被删除了
                    qDebug() << "Can't find uuid.";
                }
                break;
            }
            case DELETE_RECORD: {
                if (uuid_map.find(record_uuid) != uuid_map.end()) {
                    Record *record = uuid_map[record_uuid];
                    MultipleRecord *mr = data->records[record->get_date()];
                    for (int i = 0; i < mr->size(); i++) {
                        if (record->get_id() == (*mr)[i]->get_id()) {
                            mr->delete_record(i);
                            break;
                        }
                    }
                    if (mr->size() == 0) {
                        data->records.erase(record->get_date());
                    }
                    // emit data->record_deleted(record);
                    db_delete_record(data, record, false);
                } else {
                    // 可能是已经被删除了
                    qDebug() << "Can't find uuid.";
                }
                break;
            }
        }
    }
    emit data->all_record_changed();
}

// 同步 records 数据
// 请勿单独调用该函数，务必确保调用该函数前调用过 sync_mmods_data
void sync_records_data(Data *data) {
    QString token = get_value("token");
    if (token == "") {
        // 当前未登录
        finish_load();
        return;
    }
    Network *n = new Network("https://geomedraw.com/qt/commit_record_change");
    n->add_data("token", token);
    n->add_data("latest_record_operation", get_value("latest_record_operation", true));

    QSqlQuery query(db);
    query.prepare("SELECT id FROM record_operations WHERE uuid = :uuid");
    query.bindValue(":uuid", get_value("latest_record_operation", true));
    int latestOperationId = 0;
    if (!query.exec() || !query.next()) {
        qWarning() << "Query Error or Operation not found:" << query.lastError().text();
    } else {
        latestOperationId = query.value("id").toInt();
    }
    query.prepare("SELECT * FROM record_operations WHERE id > :id");
    query.bindValue(":id", latestOperationId);
    if (!query.exec()) {
        qWarning() << "Query Error:" << query.lastError().text();
        finish_load();
        return;
    }
    // 将查询结果转换为 JSON 数组
    QJsonArray jsonArray;
    while (query.next()) {
        QJsonObject jsonObj;
        jsonObj["uuid"] = query.value("uuid").toString();
        jsonObj["time"] = query.value("time").toString();
        jsonObj["operation_type"] = query.value("operation_type").toInt();
        jsonObj["record_uuid"] = query.value("record_uuid").toString();
        jsonObj["record_type"] = query.value("record_type").toInt();
        jsonObj["record_class"] = query.value("record_class").toInt();
        jsonObj["point"] = query.value("point").toInt();
        jsonObj["record_info"] = query.value("record_info").toString();
        jsonObj["record_date"] = query.value("record_date").toString();
        jsonObj["sort_order"] = query.value("sort_order").toInt();
        jsonArray.append(jsonObj);
    }
    n->add_data("record_operations_data", QJsonDocument(jsonArray).toJson());
    n->post();
    QObject::connect(n, &Network::succeeded, [=] (QString reply) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();
            bool succeed = jsonObj["succeed"].toBool();
            if (!succeed) {
                qDebug() << jsonObj["error_message"].toString();
                finish_load();
                return;
            }
            QJsonArray operations = jsonObj["operations"].toArray();
            merge_record_operations((Data *) data, operations);
            QString last_uuid = jsonObj["last_uuid"].toString();
            if (last_uuid != "") {
                save_value("latest_record_operation", last_uuid, true);
            }
        } else {
            qDebug() << "Failed to parse JSON.";
        }
        finish_load();
    });
    QObject::connect(n, &Network::failed, [=] () {
        finish_load();
    });
    return;
}

void db_save_record_operation(Data *data, Record *record, enum RECORD_OPERATION_TYPE operation_type) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO record_operations (uuid, user, operation_type, record_uuid, "
                  "record_type, record_class, point, record_info, record_date, sort_order) "
                  "VALUES (:uuid, :username, :operation_type, :record_uuid, "
                  ":record_type, :record_class, :point, :record_info, :record_date, :sort_order)");

    query.bindValue(":uuid", QUuid::createUuid().toString(QUuid::WithoutBraces));
    query.bindValue(":username", username);
    query.bindValue(":operation_type", (int) operation_type);
    query.bindValue(":record_uuid", record->get_uuid().toString(QUuid::WithoutBraces));
    query.bindValue(":record_type", (int) record->get_type());
    query.bindValue(":record_class", (int) record->get_class());
    query.bindValue(":point", record->get_point());
    query.bindValue(":record_info", record->to_string());
    query.bindValue(":record_date", record->get_date());
    query.bindValue(":sort_order", record->get_sort_order());

    if (!query.exec()) {
        qDebug() << "Error inserting reccord_operation:" << query.lastError().text();
    }

    sync_data(data);
}

int db_add_record(Data *data, Record *record, bool record_operation) {
    qDebug() << "Start adding record.";
    QSqlQuery query(db);

    query.prepare("INSERT INTO records (uuid, user, date, sort_order, record_class, record_type, point, record_info) "
                  "VALUES (:uuid, :username, :date, :sort_order, :record_class, :record_type, :point, :record_info)");

    query.bindValue(":uuid", record->get_uuid().toString(QUuid::WithoutBraces));
    query.bindValue(":username", username);
    query.bindValue(":date", record->get_date().toString(Qt::ISODate));
    query.bindValue(":sort_order", 0);
    query.bindValue(":record_class", (int) record->get_class());
    query.bindValue(":record_type", (int) record->get_type());
    query.bindValue(":point", record->get_point());
    query.bindValue(":record_info", record->to_string());


    if (!query.exec()) {
        qDebug() << "Error inserting record:" << query.lastError().text();
        return -1;
    }
    int id = query.lastInsertId().toInt();
    qDebug() << "End adding record, id =" << id;
    if (record_operation) {
        db_save_record_operation(data, record, ADD_RECORD);
    }
    return id;
}
bool db_modify_record(Data *data, Record *record, bool record_operation) {
    qDebug() << "Start modifying record.";
    QSqlQuery query(db);

    query.prepare("UPDATE records "
                  "SET date = :date, sort_order = :sort_order, record_class = :record_class, "
                  "record_type = :record_type, point = :point, record_info = :record_info "
                  "WHERE id = :id");

    // 绑定参数
    query.bindValue(":date", record->get_date().toString(Qt::ISODate));
    query.bindValue(":sort_order", 0);
    query.bindValue(":record_class", (int) record->get_class());
    query.bindValue(":record_type", (int) record->get_type());
    query.bindValue(":point", record->get_point());
    query.bindValue(":record_info", record->to_string());
    query.bindValue(":id", record->get_id());

    // 执行更新语句
    if (!query.exec()) {
        qDebug() << "Error updating record:" << query.lastError().text();
        return false;
    }
    qDebug() << "End modifying record.";
    if (record_operation) {
        db_save_record_operation(data, record, MODIFY_RECORD);
    }
    return true;
}
bool db_delete_record(Data *data, Record *record, bool record_operation) {
    qDebug() << "Start deleting record.";
    QSqlQuery query(db);

    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", record->get_id());

    if (!query.exec()) {
        qDebug() << "Error deleting record:" << query.lastError().text();
        return false;
    }
    qDebug() << "End deleting record.";
    if (record_operation) {
        db_save_record_operation(data, record, DELETE_RECORD);
    }
    return true;
}


enum MOD_OPERATION_TYPE {
    ADD_MOD = 1, MODIFY_MOD = 2
};

bool mods_contains(QString &uuid) {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM mod_operations WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid);
    query.exec();
    int cnt = 0;
    return query.next();
}

// 合并来自网络的模版操作记录
void merge_mod_operations(Data *data, QJsonArray &jsonArray) {
    QSqlQuery query(db);

    QString earliest_time = "9999-99-99 99:99:99";
    // 将获取到的操作记录存入数据库中
    for (const QJsonValue &value : jsonArray) {
        QJsonObject operation = value.toObject();
        query.prepare("SELECT id FROM mod_operations WHERE uuid = :uuid");
        query.bindValue(":uuid", operation["uuid"].toString());
        query.exec();
        int cnt = 0;
        while (query.next()) {
            cnt++;
        }
        if (cnt != 0) continue;

        query.prepare("INSERT INTO mod_operations (uuid, user, time, operation_type, mod_uuid, "
                      "mod_type, deleted, name, content, name_merge, formula, labels) "
                      "VALUES (:uuid, :username, :time, :operation_type, :mod_uuid, "
                      ":mod_type, :deleted, :name, :content, :name_merge, :formula, :labels)");
        query.bindValue(":uuid", operation["uuid"].toString());
        query.bindValue(":username", username);
        QString time = operation["time"].toString();
        qDebug() << time;
        if (time < earliest_time) earliest_time = time;
        query.bindValue(":time", time);
        query.bindValue(":operation_type", operation["operation_type"].toInt());
        query.bindValue(":mod_uuid", operation["mod_uuid"].toString());
        query.bindValue(":mod_type", operation["mod_type"].toInt());
        query.bindValue(":deleted", operation["deleted"].toInt() == 1);
        query.bindValue(":name", operation["name"].toString());
        query.bindValue(":content", operation["content"].toString());
        query.bindValue(":name_merge", operation["name_merge"].toString());
        query.bindValue(":formula", operation["formula"].toString());
        query.bindValue(":labels", operation["labels"].toString());
        if (!query.exec()) {
            qDebug() << "Failed to insert operation:" << query.lastError().text();
        }
    }

    std::unordered_map<QString, Mod *> uuid_map;
    for (Mod *mod : data->mods) {
        uuid_map[mod->get_uuid().toString(QUuid::WithoutBraces)] = mod;
    }

    qDebug() << earliest_time;
    query.prepare("SELECT * FROM mod_operations WHERE time >= :earliest_time ORDER BY time ASC");
    query.bindValue(":earliest_time", earliest_time);
    if (!query.exec()) {
        qDebug() << "Failed to get mod_operations:" << query.lastError().text();
    }
    while (query.next()) {
        enum MOD_OPERATION_TYPE operation_type = query.value("operation_type").toInt() == 1 ? ADD_MOD : MODIFY_MOD;
        QString name = query.value("name").toString();
        QString content = query.value("content").toString();
        QString name_merge = query.value("name_merge").toString();
        QString formula = query.value("formula").toString();
        QString labels = query.value("labels").toString();
        QString mod_uuid = query.value("mod_uuid").toString();
        enum RECORD_TYPE mod_type = query.value("mod_type").toInt() == 1 ? OBTAIN : CONSUME;
        bool deleted = query.value("deleted").toInt() == 1;
        switch (operation_type) {
            case ADD_MOD: {
                if (uuid_map.find(mod_uuid) == uuid_map.end()) {
                    Mod *mod = new Mod(0, content, new Formula(formula), mod_type, name);
                    mod->set_uuid(mod_uuid);
                    mod->set_name_merge(name_merge);
                    mod->set_labels_string(labels);
                    mod->set_deleted(deleted);
                    mod->set_id(db_add_mod(data, mod, false));
                    uuid_map[mod_uuid] = mod;
                    data->mods.push_back(mod);
                    // emit data->mod_added(mod);
                    // emit data->label_modified(mod);
                }
                break;
            }
            case MODIFY_MOD: {
                if (uuid_map.find(mod_uuid) != uuid_map.end()) {
                    Mod *mod = uuid_map[mod_uuid];
                    mod->set_short_name(name);
                    mod->set_name(content);
                    mod->set_name_merge(name_merge);
                    mod->set_deleted(deleted);
                    mod->set_labels_string(labels);
                    mod->set_type(mod_type);
                    mod->set_formula_text(formula);
                    // emit data->mod_modified(mod);
                    // emit data->label_modified(mod);
                    db_modify_mod(data, mod, false);
                } else {
                    qDebug() << "Error! Can't find uuid.";
                }
                break;
            }
        }
    }
    emit data->all_mod_changed();
}

// 同步模板数据
void sync_mods_data(Data *data) {
    QString token = get_value("token");
    if (token == "") {
        // 当前未登录
        return;
    }
    Network *n = new Network("https://geomedraw.com/qt/commit_mod_change");
    n->add_data("token", token);
    n->add_data("latest_mod_operation", get_value("latest_mod_operation", true));

    QSqlQuery query(db);
    query.prepare("SELECT id FROM mod_operations WHERE uuid = :uuid");
    query.bindValue(":uuid", get_value("latest_mod_operation", true));
    int latestOperationId = 0;
    if (!query.exec() || !query.next()) {
        qWarning() << "Query Error or Operation not found:" << query.lastError().text();
    } else {
        latestOperationId = query.value("id").toInt();
    }
    query.prepare("SELECT * FROM mod_operations WHERE id > :id");
    query.bindValue(":id", latestOperationId);
    if (!query.exec()) {
        qWarning() << "Query Error:" << query.lastError().text();
        return;
    }
    // 将查询结果转换为 JSON 数组
    QJsonArray jsonArray;
    while (query.next()) {
        QJsonObject jsonObj;
        jsonObj["uuid"] = query.value("uuid").toString();
        jsonObj["time"] = query.value("time").toString();
        jsonObj["operation_type"] = query.value("operation_type").toInt();
        jsonObj["mod_uuid"] = query.value("mod_uuid").toString();
        jsonObj["mod_type"] = query.value("mod_type").toInt();
        jsonObj["deleted"] = query.value("deleted").toInt();
        jsonObj["name"] = query.value("name").toString();
        jsonObj["content"] = query.value("content").toString();
        jsonObj["name_merge"] = query.value("name_merge").toString();
        jsonObj["formula"] = query.value("formula").toString();
        jsonObj["labels"] = query.value("labels").toString();
        jsonArray.append(jsonObj);
    }
    n->add_data("mod_operations_data", QJsonDocument(jsonArray).toJson());
    n->post();
    QObject::connect(n, &Network::succeeded, [=] (QString reply) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();
            bool succeed = jsonObj["succeed"].toBool();
            if (!succeed) {
                qDebug() << jsonObj["error_message"].toString();
                return;
            }
            QJsonArray operations = jsonObj["operations"].toArray();
            merge_mod_operations((Data *) data, operations);
            QString last_uuid = jsonObj["last_uuid"].toString();
            if (last_uuid != "") {
                save_value("latest_mod_operation", last_uuid, true);
            }
        } else {
            qDebug() << "Failed to parse JSON.";
        }
        emit ((Data *) data)->mod_sync_finished();
    });
    return;
}

void db_save_mod_operation(Data *data, Mod *mod, enum MOD_OPERATION_TYPE operation_type) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO mod_operations (uuid, user, operation_type, mod_uuid, "
                  "mod_type, deleted, name, content, name_merge, formula, labels) "
                  "VALUES (:uuid, :username, :operation_type, :mod_uuid, "
                  ":mod_type, :deleted, :name, :content, :name_merge, :formula, :labels)");

    query.bindValue(":uuid", QUuid::createUuid().toString(QUuid::WithoutBraces));
    query.bindValue(":username", username);
    query.bindValue(":operation_type", (int) operation_type);
    query.bindValue(":mod_uuid", mod->get_uuid().toString(QUuid::WithoutBraces));
    query.bindValue(":mod_type", (int) mod->get_type());
    query.bindValue(":deleted", (int) mod->is_deleted());
    query.bindValue(":name", mod->get_shortname());
    query.bindValue(":content", mod->get_name());
    query.bindValue(":name_merge", mod->get_name_merge());
    query.bindValue(":formula", mod->get_formula_text());
    query.bindValue(":labels", mod->get_labels_string());

    if (!query.exec()) {
        qDebug() << "Error inserting mod_operation:" << query.lastError().text();
    }

    sync_mods_data(data);
}

int db_add_mod(Data *data, Mod *mod, bool record_operation) {
    qDebug() << "Start adding mod.";
    QSqlQuery query(db);

    query.prepare("INSERT INTO mods (uuid, user, mod_type, deleted, name, content, name_merge, formula, labels) "
                  "VALUES (:uuid, :username, :mod_type, :deleted, :name, :content, :name_merge, :formula, :labels)");

    query.bindValue(":uuid", mod->get_uuid().toString(QUuid::WithoutBraces));
    query.bindValue(":username", username);
    query.bindValue(":mod_type", (int) mod->get_type());
    query.bindValue(":deleted", (int) mod->is_deleted());
    query.bindValue(":name", mod->get_shortname());
    query.bindValue(":content", mod->get_name());
    query.bindValue(":name_merge", mod->get_name_merge());
    query.bindValue(":formula", mod->get_formula_text());
    query.bindValue(":labels", mod->get_labels_string());

    if (!query.exec()) {
        qDebug() << "Error inserting mod:" << query.lastError().text();
        return -1;
    }
    int id = query.lastInsertId().toInt();
    qDebug() << "End adding mod, id =" << id;
    if (record_operation) {
        db_save_mod_operation(data, mod, ADD_MOD);
    }
    return id;
}
bool db_modify_mod(Data *data, Mod *mod, bool record_operation) {
    qDebug() << "Start modifying mod.";
    QSqlQuery query(db);

    query.prepare("UPDATE mods "
                  "SET mod_type = :mod_type, deleted = :deleted, name = :name, "
                  "content = :content, name_merge = :name_merge, formula = :formula, labels = :labels "
                  "WHERE id = :id");

    query.bindValue(":mod_type", (int) mod->get_type());
    query.bindValue(":deleted", (int) mod->is_deleted());
    query.bindValue(":name", mod->get_shortname());
    query.bindValue(":content", mod->get_name());
    query.bindValue(":name_merge", mod->get_name_merge());
    query.bindValue(":formula", mod->get_formula_text());
    query.bindValue(":labels", mod->get_labels_string());
    query.bindValue(":id", mod->get_id());

    if (!query.exec()) {
        qDebug() << "Error modifying mod:" << query.lastError().text();
        return false;
    }
    qDebug() << "End modifying mod.";
    if (record_operation) {
        db_save_mod_operation(data, mod, MODIFY_MOD);
    }
    return true;
}
bool db_delete_mod(Data *data, Mod *mod, bool record_operation) {
    qDebug() << "Start deleting mod.";
    QSqlQuery query(db);

    query.prepare("DELETE FROM mods WHERE id = :id");
    query.bindValue(":id", mod->get_id());

    if (!query.exec()) {
        qDebug() << "Error deleting mod:" << query.lastError().text();
        return false;
    }
    qDebug() << "End deleting mod.";
    return true;
}


class settings_item {
public:
    QString key, value, date;
    settings_item(QString key, QString default_value)
        : key(key), value(get_value(key, true, default_value)), date(get_value(key + "_time", true, "1970-01-01")) {}
    settings_item(QString key, QString value, QString date)
        : key(key), value(value), date(date) {}
};

void sync_settings(Data *data) {
    Network *n = new Network("https://geomedraw.com/qt/commit_settings_change");
    n->add_data("token", get_value("token"));

    QVector<settings_item> dataArray = {{"data_unit", "1"}};
    QJsonArray jsonArray;
    for (const settings_item &item : dataArray) {
        QJsonObject jsonObject;
        jsonObject["key"] = item.key;
        jsonObject["value"] = item.value;
        jsonObject["date"] = item.date;
        jsonArray.append(jsonObject);
    }
    QJsonDocument jsonDoc(jsonArray);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

    n->add_data("settings", jsonString);
    n->post();
    QObject::connect(n, &Network::succeeded, [=] (QString reply) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply.toUtf8());
        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();
            bool succeed = jsonObj["succeed"].toBool();
            if (!succeed) {
                qDebug() << jsonObj["error_message"].toString();
                return;
            }
            QJsonArray settings = jsonObj["settings"].toArray();
            for (const QJsonValue &value : settings) {
                QJsonObject jo = value.toObject();
                save_value(jo["key"].toString(), jo["value"].toString(), true);
                save_value(jo["key"].toString() + "_time", jo["date"].toString(), true);
            }
            emit data->settings_changed();
        } else {
            qDebug() << "Failed to parse JSON.";
        }
    });
}
