// BEFORE: 非最適なデータプロセッサ
// メモリ効率・const 正確性・モダン C++ パターンなし

#include <QVector>

class DataProcessor_Before {
public:
    // ❌ 非最適: 値によるコピー（expensive）
    void loadData(QVector<float> data) {
        rawData = data;  // コピーアサイン
    }
    
    // ❌ 非最適: 変更なしにデータを返すでも const ではない
    QVector<float> getData() {
        return rawData;  // 別のコピー
    }
    
    // ❌ 非最適: 値返却
    QVector<float> getProcessed() {
        QVector<float> result = rawData;
        for (int i = 0; i < result.size(); i++) {
            result[i] *= 2.0f;  // インデックスループ（C-style）
        }
        return result;  // 別のコピー
    }
    
    // ❌ 非最適: 例外処理もエラーハンドリングもない
    void processInPlace(int factor) {
        for (int i = 0; i < rawData.size(); i++) {
            rawData[i] = rawData[i] * factor;
        }
    }
    
    // ❌ 非最適: デストラクタなし = RAII なし（QVector はいいが原則を示さない）
    
private:
    QVector<float> rawData;  // ❌ 非最適: 単純なメンバ（スマートポインタほど詳細ではないが、概説するポイント）
};

// クライアント使用例
void exampleUsage() {
    QVector<float> myData = {1.0f, 2.0f, 3.0f};
    
    DataProcessor_Before proc;
    
    // ❌ 非最適: 3 回のコピーが発生
    proc.loadData(myData);           // copy 1
    QVector<float> processed = proc.getProcessed();  // copy 2
    QVector<float> again = proc.getData();           // copy 3
    
    // 効率がわるい
}
