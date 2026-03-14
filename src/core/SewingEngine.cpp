#include "core/SewingEngine.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QDebug>

namespace PS {

SewingEngine::SewingEngine(QObject* parent)
    : QObject(parent)
{
}

SewingEngine::~SewingEngine() = default;

void SewingEngine::reportProgress(int percent, const QString& message)
{
    if (m_progressCallback) {
        m_progressCallback(percent, message.toStdString());
    }
    emit sewingProgress(percent, message);
}

Body SewingEngine::sew(const Body& body, const SewingOptions& options)
{
    emit sewingStarted();
    reportProgress(10, QObject::tr("准备缝合..."));
    
    if (!body.isValid()) {
        emit sewingFinished(false);
        return Body();
    }
    
    reportProgress(30, QObject::tr("执行缝合..."));
    
    // 在实际环境中调用: PK_BODY_sew
    Body result = body.sewn(options);
    
    reportProgress(90, QObject::tr("后处理..."));
    
    reportProgress(100, QObject::tr("缝合完成"));
    emit sewingFinished(result.isValid());
    
    return result;
}

Body SewingEngine::sewLocal(const Body& body, const std::vector<Face>& faces, const SewingOptions& options)
{
    emit sewingStarted();
    
    if (!body.isValid() || faces.empty()) {
        emit sewingFinished(false);
        return Body();
    }
    
    // 局部缝合逻辑
    // 仅缝合指定的面
    
    reportProgress(100, QObject::tr("局部缝合完成"));
    emit sewingFinished(true);
    
    return body;
}

Body SewingEngine::sewWithTolerances(const Body& body, const std::vector<double>& tolerances)
{
    Body result = body;
    
    for (double tol : tolerances) {
        SewingOptions options;
        options.tolerance = tol;
        result = sew(result, options);
    }
    
    return result;
}

std::vector<SewingIssue> SewingEngine::analyze(const Body& body, const SewingOptions& options)
{
    std::vector<SewingIssue> issues;
    
    if (!body.isValid()) {
        return issues;
    }
    
    // 分析自由边
    auto freeEdges = findFreeEdges(body);
    
    for (const auto& edge : freeEdges) {
        // 检查是否可以配对
        bool hasPair = false;
        double minGap = 1e10;
        
        for (const auto& otherEdge : freeEdges) {
            if (edge.entity() == otherEdge.entity()) continue;
            
            // 计算间隙
            double gap = (edge.endPoint().x - otherEdge.startPoint().x) * 
                        (edge.endPoint().x - otherEdge.startPoint().x);
            gap += (edge.endPoint().y - otherEdge.startPoint().y) * 
                   (edge.endPoint().y - otherEdge.startPoint().y);
            gap += (edge.endPoint().z - otherEdge.startPoint().z) * 
                   (edge.endPoint().z - otherEdge.startPoint().z);
            gap = std::sqrt(gap);
            
            if (gap < options.maxTolerance && gap < minGap) {
                hasPair = true;
                minGap = gap;
            }
        }
        
        if (!hasPair) {
            SewingIssue issue;
            issue.type = SewingIssue::GapTooLarge;
            issue.description = QObject::tr("自由边无法配对");
            issue.edge1 = edge.entity();
            issue.gapDistance = minGap;
            issue.autoFixable = false;
            issues.push_back(issue);
        } else if (minGap > options.tolerance) {
            SewingIssue issue;
            issue.type = SewingIssue::GapTooLarge;
            issue.description = QObject::tr("间隙超出默认公差(%1)").arg(minGap);
            issue.edge1 = edge.entity();
            issue.gapDistance = minGap;
            issue.autoFixable = true;
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::vector<Edge> SewingEngine::findFreeEdges(const Body& body)
{
    std::vector<Edge> freeEdges;
    
    if (!body.isValid()) {
        return freeEdges;
    }
    
    for (const auto& edge : body.edges()) {
        // 在实际环境中检查边连接的面数
        // PK_EDGE_ask_faces
        auto faces = edge.faces();
        if (faces.size() == 1) {
            freeEdges.push_back(edge);
        }
    }
    
    return freeEdges;
}

std::vector<std::pair<Edge, Edge>> SewingEngine::findCandidatePairs(const Body& body, double maxGap)
{
    std::vector<std::pair<Edge, Edge>> pairs;
    
    auto freeEdges = findFreeEdges(body);
    
    for (size_t i = 0; i < freeEdges.size(); i++) {
        for (size_t j = i + 1; j < freeEdges.size(); j++) {
            // 计算距离
            double dist = 0.0;
            
            if (dist <= maxGap) {
                pairs.emplace_back(freeEdges[i], freeEdges[j]);
            }
        }
    }
    
    return pairs;
}

std::vector<LocalSewingRegion> SewingEngine::detectSewingRegions(const Body& body)
{
    std::vector<LocalSewingRegion> regions;
    
    if (!body.isValid()) {
        return regions;
    }
    
    // 基于连通性检测缝合区域
    
    return regions;
}

Body SewingEngine::smartSew(const Body& body, double targetTolerance)
{
    reportProgress(0, QObject::tr("智能分析中..."));
    
    SewingOptions options;
    options.tolerance = targetTolerance;
    
    // 先分析
    auto issues = analyze(body, options);
    
    // 根据分析结果调整公差
    for (const auto& issue : issues) {
        if (issue.type == SewingIssue::GapTooLarge && issue.autoFixable) {
            options.maxTolerance = std::max(options.maxTolerance, issue.gapDistance * 1.5);
        }
    }
    
    reportProgress(50, QObject::tr("使用优化公差执行缝合..."));
    
    return sew(body, options);
}

} // namespace PS
