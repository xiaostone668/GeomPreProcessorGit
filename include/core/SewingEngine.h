#ifndef SEWINGENGINE_H
#define SEWINGENGINE_H

#include "geometry/PsTypes.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include <QObject>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 缝合问题
 */
struct SewingIssue {
    enum Type {
        GapTooLarge,
        EdgeMismatch,
        OrientationError,
        NonManifold,
        DegeneratedFace
    };
    
    Type type;
    QString description;
    PK_EDGE_t edge1 = PK_ENTITY_null;
    PK_EDGE_t edge2 = PK_ENTITY_null;
    double gapDistance = 0.0;
    bool autoFixable = false;
};

/**
 * @brief 局部缝合区域
 */
struct LocalSewingRegion {
    std::vector<Face> faces;
    std::vector<Edge> boundaryEdges;
    double localTolerance = 1e-6;
    bool isClosed = false;
};

/**
 * @brief 缝合引擎
 * 
 * 将多个独立的面缝合为连续的壳体或实体
 */
class SewingEngine : public QObject {
    Q_OBJECT
    
public:
    explicit SewingEngine(QObject* parent = nullptr);
    ~SewingEngine();
    
    // 全局缝合
    Body sew(const Body& body, const SewingOptions& options = SewingOptions());
    
    // 局部缝合
    Body sewLocal(const Body& body, const std::vector<Face>& faces, 
                  const SewingOptions& options = SewingOptions());
    
    // 多公差缝合
    Body sewWithTolerances(const Body& body, const std::vector<double>& tolerances);
    
    // 预分析
    std::vector<SewingIssue> analyze(const Body& body, const SewingOptions& options = SewingOptions());
    
    // 边界分析
    std::vector<Edge> findFreeEdges(const Body& body);
    std::vector<std::pair<Edge, Edge>> findCandidatePairs(const Body& body, double maxGap);
    
    // 检测缝合区域
    std::vector<LocalSewingRegion> detectSewingRegions(const Body& body);
    
    // 智能缝合
    Body smartSew(const Body& body, double targetTolerance = 1e-6);
    
    // 设置进度回调
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }
    
signals:
    void sewingStarted();
    void sewingProgress(int percent, const QString& message);
    void sewingFinished(bool success);

private:
    ProgressCallback m_progressCallback = nullProgressCallback;
    
    void reportProgress(int percent, const QString& message);
};

} // namespace PS

#endif // SEWINGENGINE_H
