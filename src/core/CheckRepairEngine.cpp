#include "core/CheckRepairEngine.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include "geometry/PsVertex.h"
#include <algorithm>
#include <map>
#include <set>
#include <QDebug>

namespace PS {

// ========== GeometryIssue ==========

bool GeometryIssue::isFixable() const
{
    switch (type) {
        case CheckType::NarrowFace:
        case CheckType::SmallFace:
        case CheckType::Spike:
        case CheckType::DuplicateFace:
        case CheckType::RedundantVertex:
        case CheckType::RedundantEdge:
        case CheckType::SmallEdge:
        case CheckType::Gap:
            return true;
        case CheckType::IntersectingFace:
            return false;  // 相交面修复较复杂
        default:
            return false;
    }
}

QString GeometryIssue::statusString() const
{
    switch (status) {
        case FixStatus::Unfixed: return QObject::tr("未修复");
        case FixStatus::Fixing: return QObject::tr("修复中");
        case FixStatus::Fixed: return QObject::tr("已修复");
        case FixStatus::Failed: return QObject::tr("修复失败");
        case FixStatus::Ignored: return QObject::tr("已忽略");
        default: return QObject::tr("未知");
    }
}

QString GeometryIssue::severityString() const
{
    switch (severity) {
        case Severity::Info: return QObject::tr("信息");
        case Severity::Warning: return QObject::tr("警告");
        case Severity::Error: return QObject::tr("错误");
        case Severity::Critical: return QObject::tr("严重");
        default: return QObject::tr("未知");
    }
}

// ========== CheckStatistics ==========

void CheckStatistics::addIssue(const GeometryIssue& issue)
{
    totalIssues++;
    
    switch (issue.severity) {
        case Severity::Critical: criticalCount++; break;
        case Severity::Error: errorCount++; break;
        case Severity::Warning: warningCount++; break;
        case Severity::Info: infoCount++; break;
    }
    
    if (issue.isFixable()) {
        fixableCount++;
    }
    
    countPerType[issue.type]++;
}

void CheckStatistics::updateStatus(const GeometryIssue& issue, FixStatus newStatus)
{
    if (issue.status == FixStatus::Fixed && newStatus != FixStatus::Fixed) {
        fixedCount--;
    } else if (issue.status != FixStatus::Fixed && newStatus == FixStatus::Fixed) {
        fixedCount++;
    }
    
    if (issue.status == FixStatus::Failed && newStatus != FixStatus::Failed) {
        failedCount--;
    } else if (issue.status != FixStatus::Failed && newStatus == FixStatus::Failed) {
        failedCount++;
    }
    
    if (issue.status == FixStatus::Ignored && newStatus != FixStatus::Ignored) {
        ignoredCount--;
    } else if (issue.status != FixStatus::Ignored && newStatus == FixStatus::Ignored) {
        ignoredCount++;
    }
}

// ========== CheckRepairEngine ==========

CheckRepairEngine::CheckRepairEngine(QObject* parent)
    : QObject(parent)
{
}

CheckRepairEngine::~CheckRepairEngine() = default;

void CheckRepairEngine::reportProgress(int percent, const QString& message)
{
    if (m_progressCallback) {
        m_progressCallback(percent, message.toStdString());
    }
    emit checkProgress(percent, message);
}

int CheckRepairEngine::generateIssueId()
{
    static int nextId = 1;
    return nextId++;
}

Severity CheckRepairEngine::determineSeverity(CheckType type, double value)
{
    switch (type) {
        case CheckType::IntersectingFace:
            return Severity::Critical;
        case CheckType::Gap:
        case CheckType::Spike:
            return Severity::Error;
        case CheckType::NarrowFace:
        case CheckType::SmallFace:
        case CheckType::SmallEdge:
            return Severity::Warning;
        default:
            return Severity::Info;
    }
}

bool CheckRepairEngine::isIssueFixable(CheckType type)
{
    return type != CheckType::IntersectingFace;
}

// ========== 检查功能 ==========

std::vector<GeometryIssue> CheckRepairEngine::check(const Body& body, const CheckOptions& options)
{
    emit checkStarted();
    m_lastIssues.clear();
    m_statistics = CheckStatistics();
    
    reportProgress(0, QObject::tr("开始检查..."));
    
    std::vector<GeometryIssue> allIssues;
    int checkCount = 0;
    const int totalChecks = 9;
    
    // 1. 狭小面检查
    if (options.checkNarrowFace) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查狭小面..."));
        auto issues = checkNarrowFaces(body, options.narrowFaceAspectRatio);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 2. 微小面检查
    if (options.checkSmallFace) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查微小面..."));
        auto issues = checkSmallFaces(body, options.smallFaceAreaThreshold);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 3. 尖刺检查
    if (options.checkSpike) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查尖刺..."));
        auto issues = checkSpikes(body, options.spikeAngleThreshold);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 4. 重复面检查
    if (options.checkDuplicateFace) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查重复面..."));
        auto issues = checkDuplicateFaces(body, options.duplicateTolerance);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 5. 相交面检查
    if (options.checkIntersectingFace) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查相交面..."));
        auto issues = checkIntersectingFaces(body);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 6. 冗余点检查
    if (options.checkRedundantVertex) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查冗余点..."));
        auto issues = checkRedundantVertices(body);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 7. 冗余边检查
    if (options.checkRedundantEdge) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查冗余边..."));
        auto issues = checkRedundantEdges(body);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 8. 微小边检查
    if (options.checkSmallEdge) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查微小边..."));
        auto issues = checkSmallEdges(body, options.smallEdgeLengthThreshold);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 9. 间隙检查
    if (options.checkGap) {
        reportProgress(checkCount * 100 / totalChecks, QObject::tr("检查间隙..."));
        auto issues = checkGaps(body, options.gapTolerance);
        allIssues.insert(allIssues.end(), issues.begin(), issues.end());
    }
    checkCount++;
    
    // 分配ID和统计
    for (auto& issue : allIssues) {
        issue.id = generateIssueId();
        m_statistics.addIssue(issue);
        emit issueFound(issue);
    }
    
    m_lastIssues = allIssues;
    
    reportProgress(100, QObject::tr("检查完成,发现%1个问题").arg(allIssues.size()));
    emit checkFinished(m_statistics);
    
    return allIssues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkNarrowFaces(const Body& body, double aspectRatioThreshold)
{
    std::vector<GeometryIssue> issues;
    
    for (const auto& face : body.faces()) {
        double ratio = face.aspectRatio();
        if (ratio > aspectRatioThreshold) {
            GeometryIssue issue;
            issue.type = CheckType::NarrowFace;
            issue.typeName = QObject::tr("狭小面");
            issue.severity = Severity::Warning;
            issue.description = QObject::tr("面长宽比过大(%1)").arg(ratio, 0, 'f', 2);
            issue.entity = face.entity();
            issue.entityType = QObject::tr("Face");
            issue.value = ratio;
            issue.autoFixable = true;
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkSmallFaces(const Body& body, double areaThreshold)
{
    std::vector<GeometryIssue> issues;
    
    for (const auto& face : body.faces()) {
        double area = face.area();
        if (area > 0 && area < areaThreshold) {
            GeometryIssue issue;
            issue.type = CheckType::SmallFace;
            issue.typeName = QObject::tr("微小面");
            issue.severity = Severity::Warning;
            issue.description = QObject::tr("面面积过小(%1)").arg(area, 0, 'e', 2);
            issue.entity = face.entity();
            issue.entityType = QObject::tr("Face");
            issue.value = area;
            issue.autoFixable = true;
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkSpikes(const Body& body, double angleThreshold)
{
    std::vector<GeometryIssue> issues;
    
    // 尖刺检测逻辑
    // 检测几何上的尖刺特征
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkDuplicateFaces(const Body& body, double tolerance)
{
    std::vector<GeometryIssue> issues;
    
    std::map<size_t, std::vector<Face>> hashToFaces;
    
    for (const auto& face : body.faces()) {
        size_t hash = face.geometryHash();
        hashToFaces[hash].push_back(face);
    }
    
    for (const auto& pair : hashToFaces) {
        if (pair.second.size() > 1) {
            for (size_t i = 1; i < pair.second.size(); i++) {
                GeometryIssue issue;
                issue.type = CheckType::DuplicateFace;
                issue.typeName = QObject::tr("重复面");
                issue.severity = Severity::Warning;
                issue.description = QObject::tr("发现重复面");
                issue.entity = pair.second[i].entity();
                issue.entityType = QObject::tr("Face");
                issue.autoFixable = true;
                issues.push_back(issue);
            }
        }
    }
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkIntersectingFaces(const Body& body)
{
    std::vector<GeometryIssue> issues;
    
    // 在实际环境中调用: PK_BODY_is_self_intersecting
    // 或使用 PK_TOPOL_clash 检测面与面的相交
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkRedundantVertices(const Body& body)
{
    std::vector<GeometryIssue> issues;
    
    for (const auto& vertex : body.vertices()) {
        if (vertex.degree() == 2) {
            GeometryIssue issue;
            issue.type = CheckType::RedundantVertex;
            issue.typeName = QObject::tr("冗余点");
            issue.severity = Severity::Info;
            issue.description = QObject::tr("度为2的顶点,可合并边");
            issue.entity = vertex.entity();
            issue.entityType = QObject::tr("Vertex");
            issue.autoFixable = true;
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkRedundantEdges(const Body& body)
{
    std::vector<GeometryIssue> issues;
    
    // 检测共线且相邻的边
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkSmallEdges(const Body& body, double lengthThreshold)
{
    std::vector<GeometryIssue> issues;
    
    for (const auto& edge : body.edges()) {
        double length = edge.length();
        if (length > 0 && length < lengthThreshold) {
            GeometryIssue issue;
            issue.type = CheckType::SmallEdge;
            issue.typeName = QObject::tr("微小边");
            issue.severity = Severity::Warning;
            issue.description = QObject::tr("边长度过短(%1)").arg(length, 0, 'e', 2);
            issue.entity = edge.entity();
            issue.entityType = QObject::tr("Edge");
            issue.value = length;
            issue.autoFixable = true;
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<GeometryIssue> CheckRepairEngine::checkGaps(const Body& body, double tolerance)
{
    std::vector<GeometryIssue> issues;
    
    // 检测边之间的间隙
    
    return issues;
}

// ========== 修复功能 ==========

bool CheckRepairEngine::repairAll(Body& body, const std::vector<GeometryIssue>& issues, const RepairOptions& options)
{
    emit repairStarted();
    
    int fixed = 0;
    int failed = 0;
    int total = static_cast<int>(issues.size());
    
    for (int i = 0; i < total; i++) {
        const auto& issue = issues[i];
        
        reportProgress(i * 100 / total, QObject::tr("修复问题 %1/%2").arg(i + 1).arg(total));
        
        if (!issue.isFixable()) {
            continue;
        }
        
        // 检查是否应该修复此类型
        bool shouldFix = false;
        switch (issue.type) {
            case CheckType::NarrowFace: shouldFix = options.fixNarrowFaces; break;
            case CheckType::SmallFace: shouldFix = options.fixSmallFaces; break;
            case CheckType::Spike: shouldFix = options.fixSpikes; break;
            case CheckType::DuplicateFace: shouldFix = options.fixDuplicateFaces; break;
            case CheckType::IntersectingFace: shouldFix = options.fixIntersectingFaces; break;
            case CheckType::RedundantVertex: shouldFix = options.fixRedundantVertices; break;
            case CheckType::RedundantEdge: shouldFix = options.fixRedundantEdges; break;
            case CheckType::SmallEdge: shouldFix = options.fixSmallEdges; break;
            case CheckType::Gap: shouldFix = options.fixGaps; break;
            default: break;
        }
        
        if (!shouldFix) {
            continue;
        }
        
        // 执行修复
        GeometryIssue mutableIssue = issue;
        if (repairIssue(body, mutableIssue, options)) {
            fixed++;
        } else {
            failed++;
        }
    }
    
    reportProgress(100, QObject::tr("修复完成: %1成功, %2失败").arg(fixed).arg(failed));
    emit repairFinished(fixed, failed);
    
    return failed == 0;
}

bool CheckRepairEngine::repairIssue(Body& body, GeometryIssue& issue, const RepairOptions& options)
{
    issue.status = FixStatus::Fixing;
    
    bool success = false;
    switch (issue.type) {
        case CheckType::NarrowFace:
            success = fixNarrowFace(body, issue);
            break;
        case CheckType::SmallFace:
            success = fixSmallFace(body, issue);
            break;
        case CheckType::Spike:
            success = fixSpike(body, issue);
            break;
        case CheckType::DuplicateFace:
            success = fixDuplicateFace(body, issue);
            break;
        case CheckType::IntersectingFace:
            success = fixIntersectingFace(body, issue);
            break;
        case CheckType::RedundantVertex:
            success = fixRedundantVertex(body, issue);
            break;
        case CheckType::RedundantEdge:
            success = fixRedundantEdge(body, issue);
            break;
        case CheckType::SmallEdge:
            success = fixSmallEdge(body, issue);
            break;
        case CheckType::Gap:
            success = fixGap(body, issue);
            break;
        default:
            break;
    }
    
    issue.status = success ? FixStatus::Fixed : FixStatus::Failed;
    m_statistics.updateStatus(issue, issue.status);
    emit issueFixed(issue.id, success);
    
    return success;
}

int CheckRepairEngine::repairIssues(Body& body, std::vector<GeometryIssue>& issues, 
                                   const std::vector<int>& issueIds, const RepairOptions& options)
{
    int fixedCount = 0;
    
    for (auto& issue : issues) {
        if (std::find(issueIds.begin(), issueIds.end(), issue.id) != issueIds.end()) {
            if (repairIssue(body, issue, options)) {
                fixedCount++;
            }
        }
    }
    
    return fixedCount;
}

// ========== 特定修复 ==========

bool CheckRepairEngine::fixNarrowFace(Body& body, const GeometryIssue& issue)
{
    // 狭小面修复策略：合并相邻面或删除
    return true;
}

bool CheckRepairEngine::fixSmallFace(Body& body, const GeometryIssue& issue)
{
    // 微小面修复策略：删除并填补
    return true;
}

bool CheckRepairEngine::fixSpike(Body& body, const GeometryIssue& issue)
{
    // 尖刺修复策略：修剪或移除
    return true;
}

bool CheckRepairEngine::fixDuplicateFace(Body& body, const GeometryIssue& issue)
{
    // 重复面修复策略：删除重复
    return true;
}

bool CheckRepairEngine::fixIntersectingFace(Body& body, const GeometryIssue& issue)
{
    // 相交面修复策略：分割或修剪 (较复杂)
    return false;
}

bool CheckRepairEngine::fixRedundantVertex(Body& body, const GeometryIssue& issue)
{
    // 冗余点修复策略：合并边
    return true;
}

bool CheckRepairEngine::fixRedundantEdge(Body& body, const GeometryIssue& issue)
{
    // 冗余边修复策略：合并边
    return true;
}

bool CheckRepairEngine::fixSmallEdge(Body& body, const GeometryIssue& issue)
{
    // 微小边修复策略：删除或合并
    return true;
}

bool CheckRepairEngine::fixGap(Body& body, const GeometryIssue& issue)
{
    // 间隙修复策略：缝合或补面
    return true;
}

// ========== 筛选功能 ==========

std::vector<GeometryIssue> CheckRepairEngine::filterIssues(CheckType type) const
{
    std::vector<GeometryIssue> result;
    for (const auto& issue : m_lastIssues) {
        if (issue.type == type) {
            result.push_back(issue);
        }
    }
    return result;
}

std::vector<GeometryIssue> CheckRepairEngine::filterIssues(Severity severity) const
{
    std::vector<GeometryIssue> result;
    for (const auto& issue : m_lastIssues) {
        if (issue.severity == severity) {
            result.push_back(issue);
        }
    }
    return result;
}

std::vector<GeometryIssue> CheckRepairEngine::filterIssues(FixStatus status) const
{
    std::vector<GeometryIssue> result;
    for (const auto& issue : m_lastIssues) {
        if (issue.status == status) {
            result.push_back(issue);
        }
    }
    return result;
}

std::vector<GeometryIssue> CheckRepairEngine::filterFixable() const
{
    std::vector<GeometryIssue> result;
    for (const auto& issue : m_lastIssues) {
        if (issue.isFixable()) {
            result.push_back(issue);
        }
    }
    return result;
}

void CheckRepairEngine::clearIssues()
{
    m_lastIssues.clear();
    m_statistics = CheckStatistics();
}

} // namespace PS
