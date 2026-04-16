# Rules の動作: Before ＆ After 比較

このドキュメントは、**Rules（ガイドライン）を適用前後の改善**を具体的に示します。

同じプログラムロジックを持つコードが、Rules 適用で**いかに改善されるか**を見ることができます。

---

## 🔍 比較 #1: メモリ管理（生ポインタ → スマートポインタ）

### Before: Rules なし（非最適）

```cpp
class DataWidget : public QWidget {
public:
    DataWidget(QWidget *parent = nullptr) : QWidget(parent) {
        // ❌ 生ポインタ + 手動メモリ管理
        m_data = new QVector<float>;
        m_processor = new DataProcessor;
    }
    
    ~DataWidget() {
        // ❌ 手動削除必須（忘れるリスク）
        delete m_data;
        delete m_processor;
    }

private:
    QVector<float> *m_data;          // ❌ 生ポインタ
    DataProcessor *m_processor;      // ❌ 生ポインタ
};
```

**問題**:
- 所有権が不明確（誰が削除？）
- デストラクタ漏れのリスク
- コピー/ムーブセマンティクス不明確

---

### After: Rules 適用（RAII パターン）

```cpp
class DataWidget : public QWidget {
Q_OBJECT  // ✅ Qt メタオブジェクト

public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget() = default;  // ✅ デストラクタ自動実装（スマートポインタが管理）

private:
    std::unique_ptr<QVector<float>> m_data;      // ✅ スマートポインタ
    std::unique_ptr<DataProcessor> m_processor;  // ✅ スマートポインタ
};

// 実装
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_data(std::make_unique<QVector<float>>())
    , m_processor(std::make_unique<DataProcessor>())
{
    // ✅ 所有権明確・自動クリーンアップ
}
```

**改善点**:
- ✅ 所有権明確（`unique_ptr` = 排他的所有）
- ✅ 自動削除（スコープ終了時）
- ✅ デストラクタ漏れなし
- ✅ ムーブセマンティクス自動サポート

---

## 🔍 比較 #2: 関数パラメータ（値コピー → const 参照）

### Before: Rules なし（効率悪い）

```cpp
void DataWidget::loadFile(QString filename) {  // ❌ 値コピー
    std::ifstream file(filename.toStdString());
    if (!file) {
        printf("Error\n");  // ❌ C-style printf
        return;
    }

    float value;
    while (file >> value) {
        m_data->push_back(value);
    }
    file.close();  // ❌ 明示的close（RAII 非活用）
}
```

**問題**:
- `QString` の不要なコピー（大きい文字列なら高コスト）
- 呼び出しのたびにコピー
- ログが printf（Qt メカニズム外）
- ファイル操作が RAII 非活用

---

### After: Rules 適用（効率的）

```cpp
void DataWidget::loadFile(const QString &filename) {  // ✅ const 参照（ゼロコスト）
    std::ifstream file(filename.toStdString());
    if (!file) {
        qWarning() << "Cannot open file:" << filename;  // ✅ Qt ログ
        return;
    }

    float value;
    while (file >> value) {
        m_data->push_back(value);
    }
    // ✅ 自動close（ファイル終了スコープで）
}
```

**改善点**:
- ✅ ゼロコピー（const 参照）
- ✅ パフォーマンス向上（特に大規模データ）
- ✅ Qt ログ機構（一貫性・フィルタリング可能）
- ✅ RAII（自動ファイルクローズ）

---

## 🔍 比較 #3: const 正確性（なし → 完全）

### Before: Rules なし（曖昧）

```cpp
class DataProcessor {
public:
    // ❌ const なし（だが状態を変更しない）
    QVector<float> getData() {
        return m_data;  // ❌ コピー返却（高コスト）
    }
    
    // ❌ const なし（パラメータをプロセス・変更）
    void processData(QVector<float> data) {
        // ...
    }
    
private:
    QVector<float> m_data;
};

// 呼び出し
DataProcessor proc;
QVector<float> result = proc.getData();  // ❌ コピー
proc.processData(result);
```

**問題**:
- Getter が const メンバではない（意図不明）
- 返却値が値コピー（高コスト）
- パラメータが値コピー（高コスト）
- 呼び出し側が意図を不明確（変更&？読込？）

---

### After: Rules 適用（const 正確性）

```cpp
class DataProcessor {
public:
    // ✅ const メンバ + const 参照返却
    const QVector<float>& getData() const {
        return m_data;  // ✅ 参照（ゼロコスト）
    }
    
    // ✅ const 参照パラメータ（読込のみ）
    void processData(const QVector<float> &data) {
        // ...（data は読込のみ）
    }
    
private:
    QVector<float> m_data;
};

// 呼び出し
const DataProcessor proc;
const QVector<float> &result = proc.getData();  // ✅ 参照（ゼロコスト）
proc.processData(result);  // ✅ const 参照（変更不可）
```

**改善点**:
- ✅ const メンバ = getter は読込のみ（明確）
- ✅ const 参照返却 = ゼロコピー
- ✅ const 参照パラメータ = 変更なし明確
- ✅ コンパイル時チェック（const 違反は実行時エラー）

---

## 🔍 比較 #4: シグナル/スロット接続

### Before: Rules なし（型チェックなし）

```cpp
class DataWidget : public QWidget {
public:
    DataWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QPushButton *btn = new QPushButton("Load");
        
        // ❌ 文字列シンタックス（コンパイル時チェックなし）
        connect(btn, SIGNAL(clicked()), this, SLOT(onLoadClicked()));
        
        // 実行時にシンボル不一致なら...エラー（実行まで分からない）
    }
    
private:
    void onLoadClicked() { }
};
```

**問題**:
- コンパイル時型チェックなし
- シンボル名の「つづり」ミスなら実行時エラー
- パフォーマンス劣化（文字列解析オーバーヘッド）
- IDE サポート弱い（リファクタリング）

---

### After: Rules 適用（PMF シンタックス）

```cpp
class DataWidget : public QWidget {
Q_OBJECT  // ✅ Qt メタオブジェクト

public:
    explicit DataWidget(QWidget *parent = nullptr);

private slots:
    void onLoadClicked();  // ✅ スロット宣言

private:
    std::unique_ptr<QPushButton> m_loadBtn;
};

// 実装
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_loadBtn(std::make_unique<QPushButton>("Load"))
{
    // ✅ PMF シンタックス（コンパイル時型チェック）
    connect(m_loadBtn.get(), &QPushButton::clicked,
            this, &DataWidget::onLoadClicked);
}

void DataWidget::onLoadClicked() {
    // ...
}
```

**改善点**:
- ✅ コンパイル時シンボルチェック（ミスは実行前に検出）
- ✅ パフォーマンス（文字列オーバーヘッドなし）
- ✅ IDE サポート完全（リファクタリング・補完）
- ✅ 型安全（シグネチャ不一致なら拒否）

---

## 📊 改善サマリー

| 要素 | Before（Rule なし） | After（Rule 適用） | 効果 |
|------|----------|----------|------|
| **メモリ管理** | 生ポインタ + 手動削除 | スマートポインタ + RAII | 安全・自動 |
| **関数パラメータ** | 値コピー | const 参照 | ゼロコスト |
| **const 正確性** | なし（曖昧） | 完全（型チェック） | 自動検証 |
| **シグナル接続** | 文字列（実行時チェック） | PMF（コンパイルチェック） | 早期エラー検出 |
| **ログ方法** | printf | qDebug/qWarning | 一貫性・フィルタリング |
| **ファイル操作** | 明示的 close | RAII（自動） | エラー安全 |

---

## 🎯 Rules が提供する価値

1. **安全性**: コンパイル時チェック増加（実行時エラー削減）
2. **パフォーマンス**: 不要なコピー排除
3. **可読性**: 意図明確（const = 読込のみ）
4. **保守性**: 自動メモリ管理（RAII）
5. **一貫性**: チーム全体で同じパターン（コードレビュー削減）

---

## ✅ 次のステップ

→ `docs/02-skills-basics.md` で **Skills** の役割を学ぶ
