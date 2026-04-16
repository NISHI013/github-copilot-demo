// === AFTER Customization: データウィジェット UI（最適化版）=========
// このバージョンは Rules + Skills を適用した改善コードです。
//
// 改善点:
//  ✅ スマートポインタ（unique_ptr）で所有権明確
//  ✅ PMF シンタックス（signals.slots の型安全接続）
//  ✅ const 正確性（パラメータ・戻り値・メンバ関数）
//  ✅ Qt ロギング（qDebug, qWarning）
//  ✅ RAII（自動リソース管理）
//  ✅ 親-子ウィジェット関係（自動削除）

#ifndef DATA_WIDGET_H
#define DATA_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVector>
#include <memory>

// ✅ カスタムシグナルとスロット
class DataWidget : public QWidget {
    Q_OBJECT  // ✅ メタオブジェクト必須

public:
    // ✅ explicit コンストラクタ
    explicit DataWidget(QWidget *parent = nullptr);
    
    // ✅ デストラクタ = default（スマートポインタが管理）
    ~DataWidget() = default;

signals:
    // ✅ カスタムシグナル（パラメータ型明示）
    void dataLoaded(const QVector<float> &data);
    void dataError(const QString &message);

private slots:
    // ✅ スロット = 暗黙のシグナルレシーバ
    void onLoadButtonClicked();
    void onInputTextChanged(const QString &text);
    void onTableSelectionChanged();

private:
    // ✅ スマートポインタ（unique_ptr）所有権明確
    std::unique_ptr<QPushButton> m_loadButton;
    std::unique_ptr<QLineEdit> m_inputEdit;
    std::unique_ptr<QTableWidget> m_dataTable;
    
    // ✅ 内部データ（スマートポインタ）
    std::unique_ptr<QVector<float>> m_currentData;
    
    // ✅ private ユーティリティ関数
    void setupUI();
    void connectSignals();
    
    // ✅ パラメータは const 参照（コピーなし）
    void displayData(const QVector<float> &data);
    
    // ✅ const メンバ関数（状態変更なし）
    bool validateInput(const QString &filename) const;
};

#endif // DATA_WIDGET_H

// =============== 実装ファイル ==================

#include "DataWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>

// ✅ ロギングカテゴリ定義
Q_LOGGING_CATEGORY(lcDataWidget, "DataWidget")

// ✅ コンストラクタ：初期化リストで所有権明確化
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_loadButton(std::make_unique<QPushButton>("Load File"))
    , m_inputEdit(std::make_unique<QLineEdit>())
    , m_dataTable(std::make_unique<QTableWidget>())
    , m_currentData(std::make_unique<QVector<float>>())
{
    // ✅ すべてのウィジェットが this を親に持つ
    m_loadButton->setParent(this);
    m_inputEdit->setParent(this);
    m_dataTable->setParent(this);
    
    setupUI();
    connectSignals();
    
    // ✅ ロギング
    qCInfo(lcDataWidget) << "DataWidget initialized";
}

void DataWidget::setupUI() {
    // ✅ メインレイアウト
    auto mainLayout = std::make_unique<QVBoxLayout>(this);
    
    // ✅ トップパネル
    auto topLayout = std::make_unique<QHBoxLayout>();
    auto fileLabel = std::make_unique<QLabel>("Filename:");
    topLayout->addWidget(fileLabel.release());  // ownership → layout
    topLayout->addWidget(m_inputEdit.get());
    topLayout->addWidget(m_loadButton.get());
    
    // ✅ テーブルウィジェット設定
    m_dataTable->setColumnCount(2);
    m_dataTable->setHorizontalHeaderLabels({"Index", "Value"});
    
    // ✅ レイアウトに追加
    mainLayout->addLayout(topLayout.release());
    mainLayout->addWidget(m_dataTable.get());
    
    setLayout(mainLayout.release());
    setWindowTitle("Data Widget");
}

void DataWidget::connectSignals() {
    // ✅ PMF シンタックス（コンパイル時型チェック）
    //    SIGNAL/SLOT は絶対禁止
    
    connect(m_loadButton.get(), &QPushButton::clicked,
            this, &DataWidget::onLoadButtonClicked);
    
    connect(m_inputEdit.get(), &QLineEdit::textChanged,
            this, &DataWidget::onInputTextChanged);
    
    connect(m_dataTable, &QTableWidget::itemSelectionChanged,
            this, &DataWidget::onTableSelectionChanged);
}

void DataWidget::onLoadButtonClicked() {
    // ✅ パラメータを const 参照で受け取る
    const QString &filename = m_inputEdit->text();
    
    if (!validateInput(filename)) {
        // ✅ Qt ロギング + シグナル発行
        qCWarning(lcDataWidget) << "Invalid filename:" << filename;
        emit dataError("Please enter a valid filename");
        return;
    }
    
    // ✅ 例外安全（RAII）
    try {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error(
                QString("Cannot open file: %1").arg(file.errorString()).toStdString()
            );
        }
        
        // ✅ スマートポインタで自動クローズ
        QTextStream stream(&file);
        
        // ✅ データを読込
        m_currentData->clear();
        while (!stream.atEnd()) {
            bool ok = false;
            float value = stream.readLine().toFloat(&ok);
            if (ok) {
                m_currentData->append(value);
            }
        }
        
        // file は QFile オブジェクト（RAII） → スコープ終了で自動クローズ
        
        // ✅ ロギング
        qCInfo(lcDataWidget) << "Loaded" << m_currentData->size() << "values from" << filename;
        
        // ✅ データ表示
        displayData(*m_currentData);
        
        // ✅ シグナル発行（リスナー通知）
        emit dataLoaded(*m_currentData);
        
    } catch (const std::exception &e) {
        qCCritical(lcDataWidget) << "Error loading file:" << e.what();
        emit dataError(QString("Error: %1").arg(e.what()));
    }
}

void DataWidget::onInputTextChanged(const QString &text) {
    // ✅ パラメータは const 参照（コピー不要）
    qCDebug(lcDataWidget) << "Filename changed to:" << text;
}

void DataWidget::onTableSelectionChanged() {
    // ✅ const メンバ関数ではない（状態変更あり）
    int selectedRow = m_dataTable->currentRow();
    qCDebug(lcDataWidget) << "Table selection changed: row" << selectedRow;
}

void DataWidget::displayData(const QVector<float> &data) {
    // ✅ パラメータを const 参照で受け取り（コピーなし）
    m_dataTable->setRowCount(data.size());
    
    // ✅ Range-for ループ（インデックスループより安全）
    int row = 0;
    for (float value : data) {
        auto indexItem = std::make_unique<QTableWidgetItem>(QString::number(row));
        auto valueItem = std::make_unique<QTableWidgetItem>(QString::number(value, 'f', 2));
        
        m_dataTable->setItem(row, 0, indexItem.release());
        m_dataTable->setItem(row, 1, valueItem.release());
        
        ++row;
    }
}

bool DataWidget::validateInput(const QString &filename) const {
    // ✅ const メンバ関数（検証のみ・状態変更なし）
    if (filename.isEmpty()) {
        return false;
    }
    
    QFile file(filename);
    return file.exists();
}

// ===== 使用例 =====
#ifdef EXAMPLE_MAIN
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // ✅ スマートポインタで管理
    auto widget = std::make_unique<DataWidget>();
    
    // ✅ シグナル接続（外部リスナー）
    QObject::connect(widget.get(), &DataWidget::dataLoaded, [](const QVector<float> &data) {
        qDebug() << "Data loaded:" << data.size() << "items";
    });
    
    QObject::connect(widget.get(), &DataWidget::dataError, [](const QString &message) {
        QMessageBox::critical(nullptr, "Error", message);
    });
    
    widget->show();
    
    return app.exec();
}
#endif
