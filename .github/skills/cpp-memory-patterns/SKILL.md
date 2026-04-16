---
name: C++ メモリ管理パターン
description: "スマートポインタ（unique_ptr、shared_ptr）、RAII、所有権設計、メモリ安全パターン"
origin: "github-copilot-demo/skills"
tags: ["cpp", "memory", "smart-pointers", "raii"]
---

# C++ メモリ管理パターン

C++17 以降のモダンメモリ管理。生ポインタは厳禁、所有権は常に明確に。

## 🎯 いつ使うか

- 動的メモリ割当（`new` 削除予定）
- 所有権が不明確なコード
- メモリリーク防止
- スマートポインタの選択（unique_ptr vs shared_ptr）
- RAII パターン実装

---

## 🔧 コアパターン

### パターン 1: unique_ptr - 排他的所有権

```cpp
// ✅ 推奨: unique_ptr（単一所有）
class Database {
public:
    explicit Database(const QString &path)
    {
        m_connection = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"));
        m_connection->setDatabaseName(path);
        m_connection->open();  // ✅ オープン
    }
    
    ~Database() = default;  // ✅ デストラクタ不要（unique_ptr が自動close＆削除）
    
    // 無効化: コピーコンストラクタ＆代入（unique_ptr の排他性）
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;
    
    // 移動セマンティクス有効
    Database(Database &&) = default;
    Database &operator=(Database &&) = default;
    
    QSqlDatabase *get() { return m_connection.get(); }  // ✅ 参照取得（所有権保持）

private:
    std::unique_ptr<QSqlDatabase> m_connection;  // ✅ 排他的所有
};

// 使用
void main() {
    Database db("mydata.db");  // ✅ スコープ開始
    db.get()->exec("SELECT * FROM users");
}  // ✅ スコープ終了 → db 自動削除 → QSqlDatabase close＆削除
```

**効果**: 所有権明確、自動削除、複数所有不可

---

### パターン 2: shared_ptr - 複数所有権

```cpp
// ✅ 使用: shared_ptr（複数所有が必要な場合）
class DataCache {
public:
    // 参照カウント共有
    void addObserver(std::shared_ptr<DataObserver> observer) {
        m_observers.push_back(observer);  // ✅ 参照カウント +1
    }
    
    void updateData(const QVector<float> &data) {
        for (auto &obs : m_observers) {
            obs->onDataChanged(data);  // ✅ observer は削除されない（参照カウント > 0）
        }
    }

private:
    std::vector<std::shared_ptr<DataObserver>> m_observers;
};

// 使用
void main() {
    auto cache = std::make_shared<DataCache>();  // ✅ shared_ptr
    {
        auto observer = std::make_shared<DataObserver>();  // ✅ 参照カウント = 1
        cache->addObserver(observer);  // ✅ 参照カウント = 2
    }  // observer スコープ終了 → 参照カウント = 1 → 削除されない
    
    cache->updateData({1.0f, 2.0f, 3.0f});  // ✅ observer は存在
}  // cache と最後の observer 削除 → 参照カウント = 0 → オブジェクト削除
```

**効果**: 複数所有権対応、循環参照注意（weak_ptr）

---

### パターン 3: RAII - Resource Acquisition Is Initialization

```cpp
// ✅ 正しい: RAII パターン
class FileHandler {
public:
    explicit FileHandler(const QString &filename)
        : m_file(filename)  // ✅ コンストラクタ: リソース取得（ファイルオープン）
    {
        if (!m_file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot open file");
        }
    }
    
    ~FileHandler() {
        // ✅ デストラクタ: リソース解放（自動）
        if (m_file.isOpen()) {
            m_file.close();
        }
    }
    
    // コピー禁止（リソース独占）
    FileHandler(const FileHandler &) = delete;
    FileHandler &operator=(const FileHandler &) = delete;
    
    // ファイル読込
    QByteArray read() {
        return m_file.readAll();
    }

private:
    QFile m_file;  // ✅ RAII: スコープ終了時自動クローズ
};

// 使用（強い保証）
void processFile(const QString &filename) {
    try {
        FileHandler file(filename);  // ✅ オープン
        auto content = file.read();
        // 処理...
    } catch (const std::exception &e) {
        // ✅ 例外発生 → スコープ終了 → デストラクタ → ファイル自動クローズ
        qWarning() << "Error:" << e.what();
    }
    // ✅ 正常終了 → スコープ終了 → デストラクタ → ファイル自動クローズ
}
```

**効果**: 例外安全（強い保証）、リソースリーク防止

---

### パターン 4: make_unique & make_shared

```cpp
// ✅ 推奨: make_unique（例外安全）
std::unique_ptr<DataProcessor> createProcessor() {
    return std::make_unique<DataProcessor>();  // ✅ make_unique
}

// ❌ 悪い: new（例外安全でない）
std::unique_ptr<DataProcessor> createProcessor_bad() {
    return std::unique_ptr<DataProcessor>(new DataProcessor());
    // 理由: 関数引数の評価順が不定（例外時リークの可能性）
}

// ✅ 推奨: make_shared
class Observer {
public:
    std::shared_ptr<DataCache> getCache() {
        return std::make_shared<DataCache>();  // ✅ 単一割当 + 参照カウント
    }
};
```

**効果**: 例外安全、単一メモリ割当（パフォーマンス向上）

---

### パターン 5: 所有権の移譲（ムーブセマンティクス）

```cpp
// ✅ 正しい: 所有権を移譲
class DataManager {
public:
    void setProcessor(std::unique_ptr<DataProcessor> proc) {
        m_processor = std::move(proc);  // ✅ 所有権移譲（proc の所有権を失う）
    }
    
    // または右辺値参照で直接受け取り
    DataManager(std::unique_ptr<DataProcessor> &&proc)
        : m_processor(std::move(proc))
    {
    }

private:
    std::unique_ptr<DataProcessor> m_processor;
};

// 使用
void main() {
    auto proc = std::make_unique<DataProcessor>();
    DataManager manager;
    manager.setProcessor(std::move(proc));  // ✅ proc は無効化（所有権移譲）
    
    // proc.use();  // ❌ コンパイルエラー（所有権喪失）
}
```

**効果**: 所有権の明示的移譲、コンパイル時チェック

---

## 📋 アンチパターン

```cpp
// ❌ 生ポインタ（所有権不明確）
DataProcessor *processor = new DataProcessor;
// 誰が削除？ いつ削除？ 例外時は？

// ❌ new ホープ（メモリリーク）
new DataProcessor;  // ポインタを保存しない → 永遠に削除されない

// ❌ manual delete（忘れるリスク）
auto ptr = new QWidget;
// ...
delete ptr;  // 例外が発生したら delete は実行されない

// ❌ shared_ptr の循環参照
class A {
    std::shared_ptr<B> m_b;
};
class B {
    std::shared_ptr<A> m_a;  // ❌ A ↔ B 循環参照 → メモリリーク
};

// ✅ 修正: weak_ptr
class B {
    std::weak_ptr<A> m_a;  // ✅ 参照カウント非加算 → 循環回避
};
```

---

## 💡 ベストプラクティス

1. **デフォルト: unique_ptr** — 単一所有が基本
2. **shared_ptr は例外**: 複数所有が本当に必要なら
3. **weak_ptr**: 循環参照を回避
4. **make_unique/make_shared**: 常に使用（例外安全）
5. **ムーブセマンティクス**: 所有権移譲を活用
6. **RAII**: コンストラクタ=取得、デストラクタ=解放

---

## 📚 関連スキル

- [qt-ui-patterns](../qt-ui-patterns/SKILL.md) — Qt オブジェクトツリー
- [error-handling](../error-handling/SKILL.md) — 例外安全
