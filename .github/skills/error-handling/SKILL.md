---
name: エラーハンドリングパターン
description: "例外・エラーコード・戻り値チェック・ロギング・リカバリー戦略"
origin: "github-copilot-demo/skills"
tags: ["cpp", "error-handling", "exceptions", "logging"]
---

# エラーハンドリングパターン

C++ 例外・戻り値チェック・Qt ロギングによる堅牢なエラーハンドリング。

## 🎯 いつ使うか

- ファイル I/O エラー処理
-  ネットワーク通信エラー制御
- エラーステータス判定
- ユーザー通知
- ログ出力とトレース
- リカバリー戦略決定

---

## 🔧 コアパターン

### パターン 1: 例外ベースのエラーハンドリング

```cpp
// ✅ カスタム例外クラス
class FileError : public std::runtime_error {
public:
    FileError(const QString &filename, const QString &reason)
        : std::runtime_error(
            QString("File error: %1 (%2)")
            .arg(filename, reason)
            .toStdString())
        , m_filename(filename)
        , m_reason(reason)
    {
    }
    
    QString filename() const { return m_filename; }
    QString reason() const { return m_reason; }

private:
    QString m_filename;
    QString m_reason;
};

// ✅ 例外を throws する関数
class FileReader {
public:
    // @throws FileError
    QByteArray readFile(const QString &filename) const {
        QFile file(filename);
        
        if (!file.exists()) {
            throw FileError(filename, "File not found");
        }
        
        if (!file.open(QIODevice::ReadOnly)) {
            throw FileError(filename, file.errorString());
        }
        
        QByteArray content = file.readAll();
        if (file.error() != QFile::NoError) {
            throw FileError(filename, file.errorString());
        }
        
        return content;
    }
};

// ✅ try-catch でハンドリング
void processUserFile(const QString &filename) {
    FileReader reader;
    
    try {
        QByteArray data = reader.readFile(filename);
        processData(data);
        qInfo() << "File processed successfully:" << filename;
    } catch (const FileError &e) {
        // ✅ 予期されたエラー
        qWarning() << "File error:" << e.reason();
        QMessageBox::warning(nullptr, "File Error",
                           QString("Cannot read %1\n%2")
                           .arg(e.filename(), e.reason()));
    } catch (const std::exception &e) {
        // ✅ 予期しないエラー
        qCritical() << "Unexpected error:" << e.what();
        QMessageBox::critical(nullptr, "Error", "An unexpected error occurred");
    }
    // ✅ スコープ終了：RAII がリソース自動解放
}
```

**効果**: エラー伝播、スタック自動クリーンアップ、予測可能な制御フロー

---

### パターン 2: 戻り値チェック（ステータスコード）

```cpp
// ✅ 結果型（Optional のような）
template<typename T>
class Result {
public:
    explicit Result(T value) : m_value(value), m_hasError(false) {}
    explicit Result(const QString &error) : m_error(error), m_hasError(true) {}
    
    bool isSuccess() const { return !m_hasError; }
    bool isError() const { return m_hasError; }
    
    T value() const {
        if (m_hasError) {
            throw std::runtime_error(m_error.toStdString());
        }
        return m_value;
    }
    
    QString error() const { return m_error; }

private:
    T m_value;
    QString m_error;
    bool m_hasError;
};

// ✅ 戻り値で結果を返す
class DatabaseConnection {
public:
    Result<QSqlDatabase> connect(const QString &dbpath) const {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbpath);
        
        if (!db.open()) {
            return Result<QSqlDatabase>(db.lastError().text());  // ✅ エラー返却
        }
        
        return Result<QSqlDatabase>(db);  // ✅ 成功返却
    }
};

// ✅ 戻り値をチェック
void establishConnection(const QString &dbpath) {
    DatabaseConnection db;
    auto result = db.connect(dbpath);
    
    if (result.isError()) {
        qWarning() << "Connection failed:" << result.error();
        return;
    }
    
    QSqlDatabase connection = result.value();
    qInfo() << "Connected to:" << dbpath;
}
```

**効果**: 例外が使えない場合のエラー伝播、非 throw 戦略

---

### パターン 3: Qt ロギング（qDebug, qWarning, qCritical）

```cpp
// ✅ ロギングレベル使い分け
class DataProcessor {
public:
    void process(const QVector<float> &data) {
        // ✅ qDebug: 開発用詳細ログ
        qDebug() << "Processing data with" << data.size() << "items";
        
        if (data.isEmpty()) {
            // ✅ qWarning: 予期される問題（継続可能）
            qWarning() << "Empty data vector, skipping processing";
            return;
        }
        
        if (data.size() > 1000000) {
            // ✅ qCritical: 重大エラー（継続不可）
            qCritical() << "Data size exceeds limit:" << data.size();
            return;
        }
        
        // 処理...
        qDebug() << "Processing completed successfully";
    }
};

// ✅ カスタムログ形式
void configureLogging() {
    // qDebug, qWarning, qCritical の出力を制御
    QtMessageHandler oldHandler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QString levelStr;
        switch (type) {
            case QtDebugMsg:
                levelStr = "[DEBUG]";
                break;
            case QtWarningMsg:
                levelStr = "[WARN ]";
                break;
            case QtCriticalMsg:
                levelStr = "[ERROR]";
                break;
            case QtFatalMsg:
                levelStr = "[FATAL]";
                break;
            default:
                levelStr = "[INFO ]";
                break;
        }
        
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString logMessage = QString("%1 %2 %3:%4 - %5")
            .arg(timestamp, levelStr)
            .arg(context.file).arg(context.line)
            .arg(msg);
        
        std::cerr << logMessage.toStdString() << std::endl;
    });
}
```

**効果**: 構造化ログ、デバッグと本番の区別、トレース可能性

---

### パターン 4: リカバリー戦略

```cpp
// ✅ リトライロジック
class NetworkClient {
public:
    static constexpr int MAX_RETRIES = 3;
    static constexpr int RETRY_DELAY_MS = 1000;
    
    bool fetchData(const QString &url, QByteArray &result) {
        for (int attempt = 0; attempt < MAX_RETRIES; ++attempt) {
            try {
                result = performRequest(url);
                qInfo() << "Successfully fetched:" << url;
                return true;
            } catch (const std::exception &e) {
                qWarning() << "Attempt" << (attempt + 1) << "failed:" << e.what();
                
                if (attempt < MAX_RETRIES - 1) {
                    // ✅ 待機 → リトライ
                    QThread::msleep(RETRY_DELAY_MS);
                    continue;
                } else {
                    // ✅ 最終試行も失敗 → エラー返却
                    qCritical() << "Failed after" << MAX_RETRIES << "attempts";
                    return false;
                }
            }
        }
        return false;
    }

private:
    QByteArray performRequest(const QString &url) const;
};

// ✅ フォールバック戦略
class DataSource {
public:
    QVector<float> getData(const QString &source) {
        // プライマリソース試行
        try {
            return getPrimaryData(source);
        } catch (const std::exception &e) {
            qWarning() << "Primary source failed:" << e.what();
        }
        
        // セカンダリソース試行
        try {
            return getSecondaryData(source);
        } catch (const std::exception &e) {
            qWarning() << "Secondary source failed:" << e.what();
        }
        
        // デフォルト値返却
        qCritical() << "All data sources failed, returning default";
        return {0.0f, 0.0f, 0.0f};
    }

private:
    QVector<float> getPrimaryData(const QString &source) const;
    QVector<float> getSecondaryData(const QString &source) const;
};
```

**効果**: 耐障害性向上、ユーザー体験改善、グレースフルデグラデーション

---

### パターン 5: 強い保証（例外安全）

```cpp
// ✅ 強い保証: 成功 or 元の状態に戻す
class Transaction {
public:
    // @throws TransactionError
    void update(const QString &key, const QString &value) {
        // 現在の状態を保存
        QString oldValue = m_data[key];
        
        try {
            // 変更を実行
            m_data[key] = value;
            
            // 検証
            validate();  // @throws TransactionError
            
            // コミット
            commit();    // @throws TransactionError
            
            qInfo() << "Transaction succeeded:" << key << "=" << value;
        } catch (const std::exception &e) {
            // ✅ 例外発生時は自動ロールバック
            m_data[key] = oldValue;  // 元の状態に復元
            
            qCritical() << "Transaction failed, rolled back:" << e.what();
            throw;  // 例外を再スロー
        }
    }

private:
    QMap<QString, QString> m_data;
    
    void validate() const;
    void commit();
};

// 使用
void applyChanges() {
    Transaction txn;
    
    try {
        txn.update("name", "John");
        txn.update("age", "30");
        qInfo() << "All changes applied";
    } catch (const std::exception &e) {
        qCritical() << "All changes rolled back:" << e.what();
        // txn はロールバック状態
    }
}
```

**効果**: トランザクション安全性、一貫性保証

---

## 📋 アンチパターン

```cpp
// ❌ エラーを無視
file.open(QIODevice::ReadOnly);  // 戻り値をチェックしない
data = file.readAll();           // ファイルが開いていない可能性

// ❌ 漠然とした例外キャッチ
try {
    process();
} catch (...) {  // ❌ 全て catch（原因不明）
    qDebug() << "Error";
}

// ❌ printf でエラー出力
if (error) {
    printf("Error occurred\n");  // ❌ Qt ロギング外
}

// ❌ 例外後のリソースリーク
try {
    auto buffer = new char[1000];
    process(buffer);
    delete buffer;
} catch (...) {
    // buffer が削除されない（リーク）
}

// ✅ 修正: RAII で自動管理
try {
    std::vector<char> buffer(1000);
    process(buffer.data());
} catch (...) {
    // vector が自動削除
}
```

---

## 💡 ベストプラクティス

1. **階層的な例外**: 基底・派生クラスを使い分け
2. **RAII**: 例外時もリソース自動クリーンアップ
3. **Qt ロギング**: qDebug/qWarning/qCritical を使用
4. **戻り値チェック**: API の戻り値を常に確認
5. **例外仕様**: `noexcept` で保証明記
6. **リカバリー戦略**: リトライ・フォールバック・デフォルト値
7. **強い保証**: どうしても失敗は元の状態に復元

---

## 📚 関連スキル

- [cpp-memory-patterns](../cpp-memory-patterns/SKILL.md) — RAII とメモリ安全
- [qt-ui-patterns](../qt-ui-patterns/SKILL.md) — ユーザー通知
