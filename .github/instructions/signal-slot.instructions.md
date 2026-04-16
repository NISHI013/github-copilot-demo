---
name: qt-signal-slot-patterns
description: "Qt signal/slot connection patterns, PMF syntax requirements, best practices, and memory safety for connections."
applyTo: "src/**/*.{cpp,h}"
---

# Qt シグナル/スロット パターン

このファイルは `src/` フォルダの C++ ファイル用のシグナル/スロットガイドラインです。

## 🔌 接続シンタックス（必須）

### ✅ PMF（Pointer-to-Member-Function）シンタックス

```cpp
// コンパイル時型チェック・型安全性・最高パフォー​​マンス
connect(sender, &DataSource::valueChanged,
        receiver, &DataWidget::onValueChanged);

// 複数パラメータ
connect(dataSource, &DataSource::dataUpdated,
        this, &DataWidget::onDataUpdated);
```

### ❌ 古い文字列シンタックス（禁止）

```cpp
// ❌ 型チェックなし・実行時エラー可能・パフォーマンス悪い
connect(sender, SIGNAL(valueChanged()), receiver, SLOT(onValueChanged()));

// ❌ 同様に悪い
connect(sender, "valueChanged()", receiver, "onValueChanged()");
```

---

## 🎯 接続パターン

### 基本接続

```cpp
class DataWidget : public QWidget {
    Q_OBJECT

public:
    DataWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_dataSource(new DataSource(this))
    {
        // ✅ PMF シンタックス
        connect(m_dataSource, &DataSource::valueChanged,
                this, &DataWidget::onValueChanged);
    }

private slots:
    void onValueChanged(float value) {
        qDebug() << "Value changed to:" << value;
    }

private:
    DataSource *m_dataSource;
};
```

### 複数パラメータ（オーバーロード解決）

```cpp
// シグナル複数シグネチャがある場合
class DataSource : public QObject {
    Q_OBJECT

signals:
    void valueChanged(float);
    void valueChanged(int);
    void dataUpdated(float value, int timestamp);
};

// 接続時は明示的に型指定
connect(source, QOverload<float>::of(&DataSource::valueChanged),
        this, &DataWidget::onFloatValueChanged);

connect(source, QOverload<int>::of(&DataSource::valueChanged),
        this, &DataWidget::onIntValueChanged);
```

### ラムダ接続（最小化）

```cpp
// ✅ 簡単なロジックのみ
connect(button, &QPushButton::clicked, this, [this]() {
    qDebug() << "Button clicked";
    loadData();
});

// ❌ 複雑なロジック（スロット関数を作成）
connect(button, &QPushButton::clicked, this, [this]() {
    // 10+ 行の複雑な処理... → スロット関数に移動
});
```

---

## 👨‍👩‍👧‍👦 所有権と親-子関係

### 自動クリーンアップ（推奨）

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        // ✅ child widget（親が削除に責任）
        auto panel = new DataPanel(this);
        setCentralWidget(panel);
        
        // connect OK（子がクリーンアップ時に接続解除）
        connect(panel, &DataPanel::dataReady,
                this, &MainWindow::onDataReady);
    }
};
```

### 異なり owner（明示的切断）

```cpp
// ❌ 危険: 親-子関係でない接続
DataProcessor *processor = new DataProcessor();  // <-- 親なし
connect(processor, &DataProcessor::finished,
        this, &MainWindow::onProcessingDone);

delete processor;  // ❌ 接続のみが残る（ダングリング）

// ✅ 正しい: 親指定またはスマートポインタ
auto processor = std::make_unique<DataProcessor>();
connect(processor.get(), &DataProcessor::finished,
        this, &MainWindow::onProcessingDone);
// スコープ終了で自動削除
```

---

## 🔄 接続タイプ

```cpp
// Direct Connection（同じスレッド・即座）
connect(source, &Source::sig, receiver, &Receiver::slot,
        Qt::DirectConnection);

// Queued Connection（別スレッド・非同期）
connect(source, &Source::sig, receiver, &Receiver::slot,
        Qt::QueuedConnection);

// Auto Connection（自動選択）- デフォルト
connect(source, &Source::sig, receiver, &Receiver::slot);
```

---

## 📋 スロット関数宣言

```cpp
class DataWidget : public QWidget {
    Q_OBJECT

private slots:
    // ✅ 推奨: private slot（外部呼び出し不可）
    void onDataReady(const QVector<float> &data);
    void onErrorOccurred(const QString &message);

public slots:
    // ⚠️ 必要な場合のみ public
    void updateDisplay();
};
```

---

## ✅ ベストプラクティス

```cpp
// ✅ 明確で安全な接続設定
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_dataSource(new DataSource(this))  // 親指定
        , m_processor(new DataProcessor(this))
    {
        setupConnections();
    }

private:
    void setupConnections() {
        // ✅ PMF シンタックス・明確な接続マップ
        connect(m_dataSource, &DataSource::dataReady,
                m_processor, &DataProcessor::process);
        
        connect(m_processor, &DataProcessor::completed,
                this, &MainWindow::onProcessingDone);
    }

private slots:
    void onProcessingDone(const QVector<float> &result) {
        // 処理...
    }

private:
    DataSource *m_dataSource;
    DataProcessor *m_processor;
};
```

---

## ❌ アンチパターン（避けるべき）

```cpp
// ❌ 文字列シンタックス + 型チェックなし
connect(source, SIGNAL(sig()), receiver, SLOT(slot()));

// ❌ 循環接続（A → B → A）
connect(m_widgetA, &WidgetA::valueChanged,
        m_widgetB, &WidgetB::onValueChanged);
connect(m_widgetB, &WidgetB::valueChanged,
        m_widgetA, &WidgetA::onValueChanged);  // 無限ループ可能

// ❌ 親なしオブジェクトの接続
DataProcessor *proc = new DataProcessor();  // 親なし!
connect(proc, &DataProcessor::sig, this, &Widget::slot);
delete proc;  // ❌ 接続がダングリング

// ❌ スロット関数内の重い処理
private slots:
    void onButtonClicked() {
        for (int i = 0; i < 1000000; i++) {  // ❌ UI ブロック
            expensiveCalculation(i);
        }
    }
```

---

## ✨ シグナル定義

```cpp
class DataSource : public QObject {
    Q_OBJECT

public:
    DataSource(QObject *parent = nullptr)
        : QObject(parent) {}

signals:
    // ✅ 計算できるストレージなし
    void valueChanged(float value);
    void statusUpdated(const QString &status);
    void processCompleted();

    // ❌ シグナルに実装なし
    // void badSignal() { /* 実装はダメ */ }
};
```
