#ifndef CHECKREPAIRENGINE_H
#define CHECKREPAIRENGINE_H

#include "geometry/PsTypes.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include "geometry/PsVertex.h"
#include <QObject>
#include <QAbstractTableModel>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 几何问题结构
 */
struct GeometryIssue {
    int id = 0;                              // 问题ID
    CheckType type = CheckType::SmallFace;   // 问题类型
    QString typeName;                        // 类型名称
    Severity severity = Severity::Warning;   // 严重程度
    QString description;                     // 描述
    PK_ENTITY_t entity = PK_ENTITY_null;     // 相关实体
    QString entityType;                      // 实体类型名称
    double value = 0.0;                      // 量化值 (如面积、长度等)
    FixStatus status = FixStatus::Unfixed;   // 修复状态
    QString message;                         // 修复消息
    
    // 便捷判断
    bool isFixable() const;
    QString statusString() const;
    QString severityString() const;
};

/**
 * @brief 检查选项
 */
struct CheckOptions {
    // 启用的检查类型
    bool checkNarrowFace = true;
    bool checkSmallFace = true;
    bool checkSpike = true;
    bool checkDuplicateFace = true;
    bool checkIntersectingFace = true;
    bool checkRedundantVertex = true;
    bool checkRedundantEdge = true;
    bool checkSmallEdge = true;
    bool checkGap = true;
    
    // 阈值设置
    double narrowFaceAspectRatio = 10.0;     // 狭小面长宽比阈值
    double smallFaceAreaThreshold = 1e-6;    // 微小面面积阈值
    double smallEdgeLengthThreshold = 1e-4;  // 微小边长度阈值
    double spikeAngleThreshold = 5.0;        // 尖刺角度阈值(度)
    double gapTolerance = 1e-5;              // 间隙公差
    double duplicateTolerance = 1e-6;        // 重复面公差
    
    // 其他选项
    bool includeInfo = false;                // 是否包含信息级别
    bool stopOnError = false;                // 遇到错误是否停止
    int maxIssuesPerType = 1000;             // 每种类型最大问题数
};

/**
 * @brief 修复选项
 */
struct RepairOptions {
    bool fixNarrowFaces = true;
    bool fixSmallFaces = true;
    bool fixSpikes = true;
    bool fixDuplicateFaces = true;
    bool fixIntersectingFaces = false;  // 相交面修复较复杂，默认不自动修复
    bool fixRedundantVertices = true;
    bool fixRedundantEdges = true;
    bool fixSmallEdges = true;
    bool fixGaps = true;
    
    // 修复策略
    bool autoConfirm = false;           // 是否自动确认修复
    bool createBackup = true;           // 修复前是否创建备份
    double tolerance = 1e-6;            // 修复公差
    int maxIterations = 10;             // 最大迭代次数
    bool stopOnFailure = false;         // 修复失败是否停止
};

/**
 * @brief 检查结果统计
 */
struct CheckStatistics {
    int totalIssues = 0;
    int criticalCount = 0;
    int errorCount = 0;
    int warningCount = 0;
    int infoCount = 0;
    int fixableCount = 0;
    int fixedCount = 0;
    int failedCount = 0;
    int ignoredCount = 0;
    
    std::map<CheckType, int> countPerType;
    
    void addIssue(const GeometryIssue& issue);
    void updateStatus(const GeometryIssue& issue, FixStatus newStatus);
};

/**
 * @brief 检查修复引擎
 * 
 * 提供9种几何问题的检查和修复功能：
 * 1. 狭小面 - 长宽比过大的面
 * 2. 微小面 - 面积过小的面
 * 3. 尖刺 - 几何上的尖刺特征
 * 4. 重复面 - 几何重复的面
 * 5. 相交面 - 自相交或面相交
 * 6. 冗余点 - 度为2的顶点
 * 7. 冗余边 - 可合并的边
 * 8. 微小边 - 长度过小的边
 * 9. 间隙 - 边之间的间隙
 */
class CheckRepairEngine : public QObject {
    Q_OBJECT
    
public:
    explicit CheckRepairEngine(QObject* parent = nullptr);
    ~CheckRepairEngine();
    
    // ========== 检查功能 ==========
    
    // 全面检查
    std::vector<GeometryIssue> check(const Body& body, const CheckOptions& options = CheckOptions());
    
    // 特定检查
    std::vector<GeometryIssue> checkNarrowFaces(const Body& body, double aspectRatioThreshold = 10.0);
    std::vector<GeometryIssue> checkSmallFaces(const Body& body, double areaThreshold = 1e-6);
    std::vector<GeometryIssue> checkSpikes(const Body& body, double angleThreshold = 5.0);
    std::vector<GeometryIssue> checkDuplicateFaces(const Body& body, double tolerance = 1e-6);
    std::vector<GeometryIssue> checkIntersectingFaces(const Body& body);
    std::vector<GeometryIssue> checkRedundantVertices(const Body& body);
    std::vector<GeometryIssue> checkRedundantEdges(const Body& body);
    std::vector<GeometryIssue> checkSmallEdges(const Body& body, double lengthThreshold = 1e-4);
    std::vector<GeometryIssue> checkGaps(const Body& body, double tolerance = 1e-5);
    
    // ========== 修复功能 ==========
    
    // 修复所有问题
    bool repairAll(Body& body, const std::vector<GeometryIssue>& issues, const RepairOptions& options = RepairOptions());
    
    // 修复单个问题
    bool repairIssue(Body& body, GeometryIssue& issue, const RepairOptions& options = RepairOptions());
    
    // 批量修复
    int repairIssues(Body& body, std::vector<GeometryIssue>& issues, 
                     const std::vector<int>& issueIds, const RepairOptions& options = RepairOptions());
    
    // 特定修复
    bool fixNarrowFace(Body& body, const GeometryIssue& issue);
    bool fixSmallFace(Body& body, const GeometryIssue& issue);
    bool fixSpike(Body& body, const GeometryIssue& issue);
    bool fixDuplicateFace(Body& body, const GeometryIssue& issue);
    bool fixIntersectingFace(Body& body, const GeometryIssue& issue);
    bool fixRedundantVertex(Body& body, const GeometryIssue& issue);
    bool fixRedundantEdge(Body& body, const GeometryIssue& issue);
    bool fixSmallEdge(Body& body, const GeometryIssue& issue);
    bool fixGap(Body& body, const GeometryIssue& issue);
    
    // ========== 获取结果 ==========
    
    const std::vector<GeometryIssue>& lastIssues() const { return m_lastIssues; }
    const CheckStatistics& statistics() const { return m_statistics; }
    
    // 筛选结果
    std::vector<GeometryIssue> filterIssues(CheckType type) const;
    std::vector<GeometryIssue> filterIssues(Severity severity) const;
    std::vector<GeometryIssue> filterIssues(FixStatus status) const;
    std::vector<GeometryIssue> filterFixable() const;
    
    // 清空结果
    void clearIssues();
    
    // 设置进度回调
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }
    
signals:
    void checkStarted();
    void checkProgress(int percent, const QString& message);
    void checkFinished(const CheckStatistics& stats);
    
    void repairStarted();
    void repairProgress(int percent, const QString& message);
    void repairFinished(int fixedCount, int failedCount);
    
    void issueFound(const GeometryIssue& issue);
    void issueFixed(int issueId, bool success);

private:
    std::vector<GeometryIssue> m_lastIssues;
    CheckStatistics m_statistics;
    ProgressCallback m_progressCallback = nullProgressCallback;
    
    void reportProgress(int percent, const QString& message);
    int generateIssueId();
    
    // 辅助函数
    Severity determineSeverity(CheckType type, double value);
    bool isIssueFixable(CheckType type);
};

} // namespace PS

#endif // CHECKREPAIRENGINE_H
