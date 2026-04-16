---
name: Qt シグナル/スロット設計
description: "PMF（Pointer-to-Member-Function）シンタックス、接続パターン、マルチシグネチャ、カスタム接続"
origin: "github-copilot-demo/skills"
tags: ["qt", "signals", "slots", "pmf"]
---

# Qt シグナル/スロット設計

Qt の PMF（Pointer-to-Member-Function）シンタックスによる型安全なシグナル/スロット。

## 🎯 いつ使うか

- ウィジェット間の通信
- オブザーバーパターン実装
- カスタムシグナル定義
- 複雑な接続（マルチシグネチャ、オーバーロード）
- 接続タイプ指定（Direct, Queued, Lambda）

---

## 🔧 コアパターン

### パターン 1: 基本的な PMF シンタックス

```cpp
// ✅ 推奨: PMF シンタックス（コンパイル時型チェック）
class MainWindow : public QMainWindow {
Q_OBJECT  // ✅ 必須（メタオブジェクトシステム）

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoadButtonClicked();      // ✅ スロット宣言
    void onSaveButtonClicked();
    void onProgressUpdated(int value);

private:
    std::unique_ptr<QPushButton> m_loadBtn;
    std::unique_ptr<QPushButton> m_saveBtn;
    std::unique_ptr<QProgressBar> m_progress;
};

// 実装
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_loadBtn(new QPushButton("Load"))
    , m_saveBtn(new QPushButton("Save"))
    , m_progress(new QProgressBar)
{
    // ✅ PMF シンタックス: connect(signal_owner, &Class::signal,
    //                       slot_owner, &Class::slot)
    connect(m_loadBtn.get(), &QPushButton::clicked,
            this, &MainWindow::onLoadButtonClicked);
    
    connect(m_saveBtn.get(), &QPushButton::clicked,
            this, &MainWindow::onSaveButtonClicked);
    
    connect(m_progress.get(), &QProgressBar::valueChanged,
            this, &MainWindow::onProgressUpdated);
}

void MainWindow::onLoadButtonClicked() {
    qDebug() << "Load clicked";
}

void MainWindow::onSaveButtonClicked() {
    qDebug() << "Save clicked";
}

void MainWindow::onProgressUpdated(int value) {
    qDebug() << "Progress:" << value;
}

// ❌ 古い文字列シンタックス（非推奨）
// connect(m_loadBtn.get(), SIGNAL(clicked()), this, SLOT(onLoadButtonClicked()));
// 問題: コンパイル時チェックなし、つづり間違いなら実行時エラー
```

**効果**: コンパイル時シンボルチェック、型安全、IDE サポート完全

---

### パターン 2: マルチシグネチャ & オーバーロード

```cpp
// ✅ 複数シグネチャに対応
class DataWidget : public QWidget {
Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);

signals:
    void dataLoaded(const QVector<float> &data);
    void errorOccurred(const QString &message);

private slots:
    // ❌ オーバーロード: QLineEdit::textChanged(const QString&)
    void onInputChanged(const QString &text);
    
    // QComboBox::currentIndexChanged(int) & currentIndexChanged(const QString &)
    void onComboIndexChanged(int index);
    void onComboTextChanged(const QString &text);

private:
    std::unique_ptr<QLineEdit> m_input;
    std::unique_ptr<QComboBox> m_combo;
};

// 実装
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_input(new QLineEdit)
    , m_combo(new QComboBox)
{
    // QLineEdit::textChanged は単一シグネチャ
    connect(m_input.get(), &QLineEdit::textChanged,
            this, &DataWidget::onInputChanged);
    
    // QComboBox::currentIndexChanged はオーバーロード
    // ✅ QOverload<int>::of で明示的に int シグネチャを選択
    connect(m_combo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DataWidget::onComboIndexChanged);
    
    // ✅ QOverload<const QString&>::of で const QString& シグネチャを選択
    connect(m_combo.get(), QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
            this, &DataWidget::onComboTextChanged);
}

void DataWidget::onInputChanged(const QString &text) {
    qDebug() << "Input:" << text;
}

void DataWidget::onComboIndexChanged(int index) {
    qDebug() << "Combo index:" << index;
}

void DataWidget::onComboTextChanged(const QString &text) {
    qDebug() << "Combo text:" << text;
}
```

**効果**: 複雑なシグネチャ処理、明示的型指定

---

### パターン 3: カスタムシグナル & 接続チェーン

```cpp
// ✅ カスタムシグナル定義
class DataProcessor : public QObject {
Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);
    void processFile(const QString &filename);

signals:
    // ✅ カスタムシグナル（パラメータ型明示）
    void processingStarted(const QString &filename);
    void progressUpdated(int percentage);
    void processingFinished(const QVector<float> &result);
    void processingError(const QString &message);

private slots:
    void onLoadComplete(const QVector<float> &data);

private:
    std::unique_ptr<QThread> m_thread;
};

// 実装
DataProcessor::DataProcessor(QObject *parent)
    : QObject(parent)
{
}

void DataProcessor::processFile(const QString &filename) {
    emit processingStarted(filename);  // ✅ シグナル発行
    
    // 処理...
    emit progressUpdated(50);
    
    // 完了
    QVector<float> result = {1.0f, 2.0f, 3.0f};
    emit processingFinished(result);
}

// 使用例
class MainWindow : public QMainWindow {
Q_OBJECT

private slots:
    void onProcessingStarted(const QString &filename) {
        qDebug() << "Processing:" << filename;
    }
    
    void onProgressUpdated(int percentage) {
        m_progressBar->setValue(percentage);
    }
    
    void onProcessingFinished(const QVector<float> &result) {
        qDebug() << "Result count:" << result.size();
    }
    
    void onProcessingError(const QString &message) {
        QMessageBox::critical(this, "Error", message);
    }

private:
    MainWindow() {
        auto processor = new DataProcessor(this);  // ✅ parent = this
        
        // ✅ シグナルチェーン: processor の複数シグナルを接続
        connect(processor, &DataProcessor::processingStarted,
                this, &MainWindow::onProcessingStarted);
        connect(processor, &DataProcessor::progressUpdated,
                this, &MainWindow::onProgressUpdated);
        connect(processor, &DataProcessor::processingFinished,
                this, &MainWindow::onProcessingFinished);
        connect(processor, &DataProcessor::processingError,
                this, &MainWindow::onProcessingError);
        
        processor->processFile("data.txt");
    }
};
```

**効果**: 明確なイベント通信、カップリング低減

---

### パターン 4: 接続タイプ & スレッド間通信

```cpp
// ✅ 接続タイプ明示（デフォルト: Direct）
class WorkerThread : public QObject {
Q_OBJECT

signals:
    void resultReady(const QString &result);

public slots:
    void doWork() {
        // ワーカースレッド内で処理
        QString result = "Work completed";
        emit resultReady(result);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // ワーカースレッド
    QThread thread;
    WorkerThread worker;
    worker.moveToThread(&thread);
    
    // ✅ Qt::QueuedConnection：スレッド間通信（スレッドセーフ）
    // シグナル は worker スレッドで発行
    // スロット は main スレッドの event loop で実行
    QMainWindow mainWindow;
    connect(&worker, &WorkerThread::resultReady,
            &mainWindow, [](const QString &result) {
                qDebug() << "Main thread:" << result;
            }, Qt::QueuedConnection);  // ✅ スレッドセーフ接続
    
    // ✅ Qt::DirectConnection：直接実行（スレッド同一なら）
    connect(&worker, &WorkerThread::resultReady,
            [](const QString &result) {
                qDebug() << "Direct:" << result;
            }, Qt::DirectConnection);  // ✅ オーバーヘッドなし
    
    // ワーカー起動
    thread.start();
    QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    
    return app.exec();
}
```

**効果**: スレッドセーフ通信、オーバーヘッド制御

---

### パターン 5: Lambda スロット & 接続制御

```cpp
// ✅ Lambda スロット（C++11+）
class Button : public QPushButton {
Q_OBJECT

public:
    explicit Button(QWidget *parent = nullptr)
        : QPushButton("Click me", parent)
    {
        int counter = 0;
        
        // ✅ Lambda で状態をキャプチャ（値キャプチャ [counter] 推奨）
        connect(this, &Button::clicked, [counter]() mutable {
            counter++;
            qDebug() << "Clicked" << counter << "times";
        });
    }
};

// ✅ 接続を制御 & 切断
class DataMonitor : public QObject {
Q_OBJECT

private:
    bool m_monitoringEnabled = false;
    QMetaObject::Connection m_dataConnection;  // ✅ 接続を保存
    
public slots:
    void onDataUpdated(const QVector<float> &data) {
        if (m_monitoringEnabled) {
            qDebug() << "Data count:" << data.size();
        }
    }
    
    void enableMonitoring(DataSource *source) {
        if (!m_monitoringEnabled) {
            // ✅ 接続を保存（後で切断可能）
            m_dataConnection = connect(source, &DataSource::dataUpdated,
                                      this, &DataMonitor::onDataUpdated);
            m_monitoringEnabled = true;
        }
    }
    
    void disableMonitoring() {
        if (m_monitoringEnabled) {
            // ✅ 接続を明示的に切断
            disconnect(m_dataConnection);
            m_monitoringEnabled = false;
        }
    }
};
```

**効果**: 柔軟な接続制御、ダイナミックな enable/disable

---

## 📋 アンチパターン

```cpp
// ❌ 古い文字列シンタックス（実行時エラーリスク）
connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));

// ❌ 接続タイプ省略（スレッド間通信が危険）
connect(worker, &Worker::finished, this, &MainWindow::onFinished);
// → Qt::DirectConnection がデフォルト（スレッド非同期の場合リスク）

// ❌ Lambda で参照キャプチャ（ダングリング参照）
auto btn = new QPushButton;
connect(btn, &QPushButton::clicked, [&btn]() {
    btn->setText("Clicked");  // ❌ btn がスコープ外で削除される可能性
});

// ❌ 接続を切断しない（シグナルリーク）
auto source = new DataSource;
connect(source, &DataSource::dataReady, this, &MainWindow::onDataReady);
// source が削除されても接続は残る（メモリリーク）
```

---

## 💡 ベストプラクティス

1. **PMF シンタックスのみ**: 文字列シンタックスは使用禁止
2. **Q_OBJECT マクロ**: シグナル/スロット クラスに必須
3. **スロット宣言**: `private slots:` セクションに明示
4. **オーバーロード**: `QOverload<T>::of` で明示的に選択
5. **接続タイプ**: スレッド間 → `Qt::QueuedConnection`
6. **接続保存**: 切断予定 → `QMetaObject::Connection` に保存
7. **Lambda**: 値キャプチャ `[var]` を推奨（参照は避ける）

---

## 📚 関連スキル

- [qt-ui-patterns](../qt-ui-patterns/SKILL.md) — ウィジェット構成
- [error-handling](../error-handling/SKILL.md) — 例外安全
