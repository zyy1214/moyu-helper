#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

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
QString get_value(QString key, bool user_specific) {
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
        return "";
    }
    if (query.next()) {
        QString retrievedValue = query.value(0).toString();
        //qDebug() << "Retrieved value for key" << key << ":" << retrievedValue;
        kv_cache[key] = retrievedValue;
        return retrievedValue;
    } else {
        qDebug() << "Key" << key << "not found in the database.";
        kv_cache[key] = "";
    }
    return "";
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
    query_create.exec("CREATE INDEX IF NOT EXISTS user_index ON mods (user)");


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
            mr->push_back(record);
        } else {
            qDebug() << "error: record is not initialized!";
        }
    }
    if (mr) {
        data->records[*last] = mr;
    }

    sync_mods_data(data);
}

int db_add_record(Record *record) {
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
    return id;
}

bool db_modify_record(Record *record) {
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
    return true;
}

bool db_delete_record(Record *record) {
    qDebug() << "Start deleting record.";
    QSqlQuery query(db);

    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", record->get_id());

    if (!query.exec()) {
        qDebug() << "Error deleting record:" << query.lastError().text();
        return false;
    }
    qDebug() << "End deleting record.";
    return true;
}


enum MOD_OPERATION_TYPE {
    ADD = 1, MODIFY = 2
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
        query.bindValue(":deleted", operation["deleted"].toBool());
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
        enum MOD_OPERATION_TYPE operation_type = query.value("operation_type").toInt() == 1 ? ADD : MODIFY;
        QString name = query.value("name").toString();
        QString content = query.value("content").toString();
        QString name_merge = query.value("name_merge").toString();
        QString formula = query.value("formula").toString();
        QString labels = query.value("labels").toString();
        QString mod_uuid = query.value("mod_uuid").toString();
        enum RECORD_TYPE mod_type = query.value("mod_type").toInt() == 1 ? OBTAIN : CONSUME;
        bool deleted = query.value("deleted").toBool();
        switch (operation_type) {
            case ADD: {
                if (uuid_map.find(mod_uuid) == uuid_map.end()) {
                    Mod *mod = new Mod(0, content, new Formula(formula), mod_type, name);
                    mod->set_uuid(mod_uuid);
                    mod->set_name_merge(name_merge);
                    mod->set_labels_string(labels);
                    mod->set_id(db_add_mod(data, mod, false));
                    mod->set_deleted(deleted);
                    uuid_map[mod_uuid] = mod;
                    data->mods.push_back(mod);
                    emit data->mod_added(mod);
                }
                break;
            }
            case MODIFY: {
                if (uuid_map.find(mod_uuid) != uuid_map.end()) {
                    Mod *mod = uuid_map[mod_uuid];
                    mod->set_short_name(name);
                    mod->set_name(content);
                    mod->set_name_merge(name_merge);
                    mod->set_deleted(deleted);
                    mod->set_labels_string(labels);
                    mod->set_type(mod_type);
                    mod->set_formula_text(formula);
                    emit data->mod_modified(mod);
                    db_modify_mod(data, mod, false);
                } else {
                    qDebug() << "Error! Can't find uuid.";
                }
                break;
            }
        }
    }
}

// 同步模板数据
void sync_mods_data(Data *data) {
    QString token = get_value("token");
    if (token == "") {
        // 当前未登录
        return;
    }
    Network *n = new Network(data, "https://geomedraw.com/qt/commit_mod_change");
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
        jsonObj["deleted"] = query.value("deleted").toBool();
        jsonObj["name"] = query.value("name").toString();
        jsonObj["content"] = query.value("content").toString();
        jsonObj["name_merge"] = query.value("name_merge").toString();
        jsonObj["formula"] = query.value("formula").toString();
        jsonObj["labels"] = query.value("labels").toString();
        jsonArray.append(jsonObj);
    }
    n->add_data("mod_operations_data", QJsonDocument(jsonArray).toJson());
    n->post([=] (void *data, QString reply) {
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
    }, [] (QMainWindow *window) {});
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
        db_save_mod_operation(data, mod, ADD);
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
        db_save_mod_operation(data, mod, MODIFY);
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
