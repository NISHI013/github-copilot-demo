---
name: Qt UI パターン
description: "Qt UI コンポーネントの構成、レイアウト設計、ウィジェット階層パターン"
origin: "github-copilot-demo/skills"
tags: ["qt", "ui", "widgets", "layout"]
---

# Qt UI パターン

実践的な Qt グラフィカルユーザーインターフェース設計パターン。

## 🎯 いつ使うか

- UI ウィジェットの階層設計
- レイアウトとスペーシング
- ウィジェット間の所有権関係
- 複雑なダイアログ・パネルの構成
- 親-子ウィジェット管理

---

## 🔧 コアパターン

### パターン 1: 親-子ウィジェット所有権（自動クリーンアップ）

```cpp
// ✅ 正しい: 親がウィジェットを所有（Qt オブジェクトツリーが管理）
class DataPanel : public QWidget {
Q_OBJECT

public:
    explicit DataPanel(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        auto layout = new QVBoxLayout(this);  // ✅ this が親 → 自動削除
        
        auto label = new QLabel("Data", this);  // ✅ this が親 → 親削除時に自動削除
        auto edit = new QLineEdit(this);        // ✅ this が親 → 自動削除
        auto btn = new QPushButton("OK", this); // ✅ this が親 → 自動削除
        
        layout->addWidget(label);
        layout->addWidget(edit);
        layout->addWidget(btn);
        
        setLayout(layout);
    }
    
    ~DataPanel() = default;  // ✅ デストラクタ不要（オブジェクトツリーが管理）
};

// ❌ 間違い: 親指定なし（メモリリーク）
auto panel = new DataPanel;
panel->show();
// panel が parent を持たない → 明示的 delete 必要（忘れるリスク）
```

**効果**: 自動メモリ管理（Qt オブジェクトツリー）

---

### パターン 2: カスタム レイアウト構成

```cpp
// ✅ 推奨: レイアウトを初期化リストで設定
class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        auto central = new QWidget(this);
        setCentralWidget(central);
        
        // 複数レイアウトの組み合わせ
        auto mainLayout = new QVBoxLayout(central);
        
        // トップパネル
        auto topPanel = createTopPanel();
        mainLayout->addWidget(topPanel);
        
        // メインエリア（左右分割）
        auto splitter = new QSplitter(Qt::Horizontal);
        splitter->addWidget(createLeftPanel());
        splitter->addWidget(createRightPanel());
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);
        mainLayout->addWidget(splitter);
        
        // ボトムパネル（固定高さ）
        auto statusBar = new QStatusBar(this);
        setStatusBar(statusBar);
    }
    
private:
    QWidget *createTopPanel() {
        auto panel = new QWidget;
        auto layout = new QHBoxLayout(panel);
        // ...追加
        return panel;
    }
};
```

**効果**: スケーラブル UI、柔軟なレイアウト

---

### パターン 3: ウィジェット シグナル統合

```cpp
// ✅ 正しい: ウィジェットシグナルを PMF で接続
class DataWidget : public QWidget {
Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);

signals:
    void dataChanged(const QVector<float> &newData);  // ✅ カスタムシグナル

private slots:
    void onLoadButtonClicked();      // ✅ スロット
    void onLineEditTextChanged(const QString &text);

private:
    std::unique_ptr<QPushButton> m_loadBtn;
    std::unique_ptr<QLineEdit> m_input;
    std::unique_ptr<QTableWidget> m_table;
};

// 実装
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_loadBtn(new QPushButton("Load"))
    , m_input(new QLineEdit)
    , m_table(new QTableWidget)
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_loadBtn.get());
    layout->addWidget(m_input.get());
    layout->addWidget(m_table.get());
    setLayout(layout);
    
    // ✅ PMF シンタックス（型安全）
    connect(m_loadBtn.get(), &QPushButton::clicked,
            this, &DataWidget::onLoadButtonClicked);
    connect(m_input.get(), &QLineEdit::textChanged,
            this, &DataWidget::onLineEditTextChanged);
}

void DataWidget::onLoadButtonClicked() {
    // データ読込処理
    QVector<float> data = { 1.0f, 2.0f, 3.0f };
    emit dataChanged(data);  // ✅ カスタムシグナル発行
}

void DataWidget::onLineEditTextChanged(const QString &text) {
    // テキスト変更処理
}
```

**効果**: 型安全なシグナル/スロット、モジュール間通信

---

### パターン 4: スタイルシート＆ 見た目カスタマイズ

```cpp
// ✅ 正しい: スタイルシートで一括スタイリング
void setupStyles(QApplication &app) {
    const QString stylesheet = R"(
        QMainWindow {
            background-color: #f0f0f0;
        }
        QPushButton {
            background-color: #0078d4;
            color: white;
            padding: 5px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #1084d8;
        }
        QPushButton:pressed {
            background-color: #006abc;
        }
        QLineEdit {
            border: 1px solid #ccc;
            padding: 3px;
        }
    )";
    
    app.setStyleSheet(stylesheet);  // ✅ アプリケーション全体に適用
}

// 使用
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    setupStyles(app);
    
    MainWindow w;
    w.show();
    return app.exec();
}
```

**効果**: 一貫したビジュアル、保守性向上

---

## 📋 アンチパターン

```cpp
// ❌ 親指定なし（オブジェクトツリー外）
auto btn = new QPushButton("Save");  // 親なし = 明示的削除必要

// ❌ レイアウト手動レイアウト
QVBoxLayout layout;  // スタック上 → ウィジェット追加時ダングリング
layout.addWidget(new QLabel("Text"));

// ❌ 古い文字列シンタックス
connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));

// ❌ ウィジェット削除し忘れ
auto panel = new DataPanel(parent);
panel->hide();
// panel は生ポインタ → 誰が削除？
```

---

## 💡 ベストプラクティス

1. **常に親を指定**: `new QWidget(parent_widget)` パターン
2. **レイアウトを親に渡す**: `new QVBoxLayout(this)`
3. **PMF シンタックス**: `connect(signal, &Class::slot)`
4. **スマートポインタ（オプション）**: 複雑な所有権なら `std::unique_ptr`
5. **スタイルシート**: CSS ベース（ハードコード回避）
6. **シグナルカスタム**: 公開インターフェース定義

---

## 📚 関連スキル

- [cpp-memory-patterns](../cpp-memory-patterns/SKILL.md) — スマートポインタ管理
- [qt-signal-slot-design](../qt-signal-slot-design/SKILL.md) — PMF シグナル設計
