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
void save_value(QString key, QString value) {
    init_db_kv();
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
QString get_value(QString key) {
    init_db_kv();
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

int db_add_mod(Mod *mod) {
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
    return id;
}
bool db_modify_mod(Mod *mod) {
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
    return true;
}
bool db_delete_mod(Mod *mod) {
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
