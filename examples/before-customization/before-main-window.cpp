// BEFORE: カスタマイズなし
// このコードは最適化されておらず、デモで示される最良実践に従いません
// 「❌ 非最適」マークが付いている部分に注目してください

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>
#include <fstream>

class DataProcessor {  // ❌ 非最適: クラス名がやや曖昧
public:
    void processData(QVector<float> data) {  // ❌ 非最適: 値でコピー（高コスト）
        for (int i = 0; i < data.size(); i++) {  // ❌ 非最適: インデックスループ（C-style）
            data[i] = data[i] * 2.0f;
        }
        this->data = data;  // ❌ 非最適: 冗長なコピー割り当て
    }
    
    QVector<float> getData() {  // ❌ 非最適: 値を返す（コピーコスト）
        return this->data;
    }
    
private:
    QVector<float> data;
};

class DataWidget : public QWidget {
public:
    DataWidget(QWidget *parent = nullptr) : QWidget(parent) {
        // ❌ 非最適: 生ポインタで所有権管理
        m_data = new QVector<float>;
        m_processor = new DataProcessor;
        
        // UI 設定
        auto layout = new QVBoxLayout();  // ❌ 非最適: 生ポインタ
        
        QPushButton *loadBtn = new QPushButton("Load");
        QLabel *statusLabel = new QLabel("No data");
        
        layout->addWidget(statusLabel);
        layout->addWidget(loadBtn);
        setLayout(layout);
        
        // ❌ 非最適: 古い文字列シンタックス（コンパイル時型チェックなし）
        connect(loadBtn, SIGNAL(clicked()), this, SLOT(onLoadClicked()));
    }
    
    ~DataWidget() {
        // ❌ 非最適: 手動削除（RAII 破棄）
        delete m_data;
        delete m_processor;
        // layouts・buttons・labels is handled by Qt parent-child
    }
    
    void loadFile(QString filename) {  // ❌ 非最適: const 参照ではなく値コピー
        std::ifstream file(filename.toStdString());
        if (!file.is_open()) {
            printf("Error opening file\n");  // ❌ 非最適: printf；Qt ログ機構を使わない
            return;
        }
        
        // ファイル処理
        float value;
        while (file >> value) {
            m_data->push_back(value);
        }
        file.close();  // ❌ 非最適: 明示的関閉（RAII の恩恵なし）
        
        m_processor->processData(*m_data);
    }
    
private:
    // ❌ 非最適: 生ポインタ（所有権不明確）
    QVector<float> *m_data;
    DataProcessor *m_processor;
    
    void onLoadClicked() {  // ❌ 非最適: const ではない（副作用がない場合も）
        loadFile("data.txt");
    }
};
