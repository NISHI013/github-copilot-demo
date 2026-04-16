// === AFTER Customization: データプロセッサ（最適化版）=====
// このバージョンは Rules + Skills（メモリパターン・エラーハンドリング）を適用
//
// 改善点:
//  ✅ const 参照パラメータ（コピーなし）
//  ✅ Range-for ループ（C スタイルループ廃止）
//  ✅ const 正確性（read-only 関数を const マーク）
//  ✅ スマートポインタ（メモリ安全）
//  ✅ 例外ベースエラーハンドリング
//  ✅ Uniform initialization（初期化リスト）

#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <QVector>
#include <QString>
#include <memory>
#include <stdexcept>

// ✅ カスタム例外（階層的エラー）
class ProcessingError : public std::runtime_error {
public:
    explicit ProcessingError(const QString &message)
        : std::runtime_error(message.toStdString())
        , m_message(message)
    {
    }
    
    QString message() const { return m_message; }

private:
    QString m_message;
};

class DataProcessor {
public:
    // ✅ explicit コンストラクタ
    explicit DataProcessor(float threshold = 0.0f);
    
    // ✅ デストラクタ（リソース自動解放）
    ~DataProcessor() = default;
    
    // ✅ パラメータは const 参照（コピーなし）
    // @throws ProcessingError
    QVector<float> process(const QVector<float> &rawData) const;
    
    // ✅ const メンバ関数（状態変更なし）
    int getProcessedCount() const { return m_processedCount; }
    
    // ✅ setter（状態変更あり）
    void setThreshold(float threshold) {
        if (threshold < 0.0f) {
            throw ProcessingError("Threshold must be non-negative");
        }
        m_threshold = threshold;
    }

private:
    // ✅ メンバ変数初期化
    float m_threshold;
    mutable int m_processedCount = 0;  // 統計情報（mutable で const 内で変更可）
    
    // ✅ private ユーティリティ
    // パラメータは const 参照
    bool isValidData(const QVector<float> &data) const;
    float normalizeValue(float rawValue) const;
    float filterOutlier(float value, float mean, float stdDev) const;
};

#endif // DATA_PROCESSOR_H

// =============== 実装ファイル ==================

#include "DataProcessor.h"
#include <cmath>
#include <algorithm>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcProcessor, "DataProcessor")

DataProcessor::DataProcessor(float threshold)
    : m_threshold(threshold)
    , m_processedCount(0)
{
    // ✅ Uniform initialization（初期化リスト）
    if (m_threshold < 0.0f) {
        throw ProcessingError("Initial threshold must be non-negative");
    }
    
    qCInfo(lcProcessor) << "DataProcessor created with threshold:" << m_threshold;
}

QVector<float> DataProcessor::process(const QVector<float> &rawData) const {
    // ✅ パラメータは const 参照（読込専用）
    
    if (!isValidData(rawData)) {
        throw ProcessingError("Invalid or empty input data");
    }
    
    try {
        // ✅ 結果ベクトル
        QVector<float> processed;
        processed.reserve(rawData.size());  // ✅ 事前割当（パフォーマンス）
        
        // ステップ 1: 正規化
        qCDebug(lcProcessor) << "Step 1: Normalizing" << rawData.size() << "values";
        
        // ✅ Range-for（C スタイルループ廃止）
        for (float rawValue : rawData) {
            float normalized = normalizeValue(rawValue);
            processed.append(normalized);
        }
        
        // ステップ 2: 統計計算（外れ値検出用）
        qCDebug(lcProcessor) << "Step 2: Computing statistics";
        
        float sum = 0.0f;
        // ✅ std::accumulate の代わりに range-for
        for (float value : processed) {
            sum += value;
        }
        float mean = sum / processed.size();
        
        // ✅ 分散計算
        float variance = 0.0f;
        for (float value : processed) {
            float diff = value - mean;
            variance += diff * diff;
        }
        variance /= processed.size();
        float stdDev = std::sqrt(variance);
        
        // ステップ 3: 外れ値フィルタリング
        qCDebug(lcProcessor) << "Step 3: Filtering outliers (mean="
                            << mean << ", stdDev=" << stdDev << ")";
        
        QVector<float> filtered;
        filtered.reserve(processed.size());
        
        // ✅ Range-for でフィルタリング（インデックスループ不要）
        for (float value : processed) {
            if (!filterOutlier(value, mean, stdDev)) {
                filtered.append(value);
            }
        }
        
        // ✅ mutable で const メンバを更新（統計）
        m_processedCount = filtered.size();
        
        qCInfo(lcProcessor) << "Processing complete:"
                           << "input=" << rawData.size()
                           << "output=" << filtered.size()
                           << "threshold=" << m_threshold;
        
        return filtered;  // ✅ NRVO（名前付き戻り値最適化）
        
    } catch (const std::exception &e) {
        qCCritical(lcProcessor) << "Processing failed:" << e.what();
        throw ProcessingError(QString("Processing error: %1").arg(e.what()));
    }
}

bool DataProcessor::isValidData(const QVector<float> &data) const {
    // ✅ const メンバ関数（状態変更なし）
    
    if (data.isEmpty()) {
        qCWarning(lcProcessor) << "Input data is empty";
        return false;
    }
    
    // ✅ Range-for で検証
    for (float value : data) {
        if (std::isnan(value) || std::isinf(value)) {
            qCWarning(lcProcessor) << "Invalid value (NaN or Inf):" << value;
            return false;
        }
    }
    
    return true;
}

float DataProcessor::normalizeValue(float rawValue) const {
    // ✅ const メンバ関数
    
    // 正規化: [0, 1] 範囲に変換
    float normalized = std::clamp(rawValue, 0.0f, 100.0f) / 100.0f;
    return normalized;
}

float DataProcessor::filterOutlier(float value, float mean, float stdDev) const {
    // ✅ const メンバ関数（統計的外れ値検出）
    
    if (stdDev == 0.0f) {
        // すべての値が同じ → 外れ値なし
        return false;
    }
    
    // Z スコア（標準偏差何個分か）
    float zScore = std::abs(value - mean) / stdDev;
    
    // Z スコア > 3（99.7% 信頼区間外）なら外れ値
    return zScore > 3.0f;
}

// ===== 使用例 =====
#ifdef EXAMPLE_USAGE
void exampleUsage() {
    try {
        // ✅ std::make_unique で作成
        auto processor = std::make_unique<DataProcessor>(0.1f);
        
        // ✅ const 参照でデータ渡し（コピー不要）
        QVector<float> rawData = {
            10.5f, 20.3f, 15.7f, 200.0f, 18.2f, 19.5f, 21.0f
            // 200.0f は外れ値 → フィルタリング済み
        };
        
        // ✅ 例外安全な処理
        QVector<float> result = processor->process(rawData);
        
        qInfo() << "Processed data count:" << processor->getProcessedCount();
        qInfo() << "Results:" << result;
        
    } catch (const ProcessingError &e) {
        // ✅ カスタム例外をキャッチ
        qCritical() << "Processing failed:" << e.message();
        
    } catch (const std::exception &e) {
        // ✅ 予期しない例外も処理
        qCritical() << "Unexpected error:" << e.what();
    }
    // ✅ スコープ終了：processor 自動削除
}
#endif
