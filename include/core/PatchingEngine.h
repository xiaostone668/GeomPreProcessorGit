#ifndef PATCHINGENGINE_H
#define PATCHINGENGINE_H

#include "geometry/PsTypes.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QObject>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 补面方法
 */
enum class PatchingMethod {
    Auto,           // 自动选择
    GeomPlate,      // 曲面拟合 (推荐)
    Coons,          // Coons曲面
    BSpline,        // B-Spline曲面
    Ruled,          // 直纹面
    Filling,        // 填充曲面
    ConstraintFilling  // 约束填充
};

/**
 * @brief 补面选项
 */
struct PatchingOptions {
    PatchingMethod method = PatchingMethod::Auto;
    int degree = 3;
    int numUPatches = 10;
    int numVPatches = 10;
    double tolerance = 1e-6;
    double maxError = 1e-3;
    int maxIterations = 100;
    bool tangency = false;
    double tangencyScale = 1.0;
    bool enforceBounds = true;
    bool optimizeKnots = true;
    bool fairing = false;
    double fairingFactor = 0.5;
};

/**
 * @brief 补面结果
 */
struct PatchingResult {
    bool success = false;
    Face face;
    double maxDeviation = 0.0;
    double avgDeviation = 0.0;
    QString errorMessage;
    int numIterations = 0;
    PatchingMethod usedMethod;
};

/**
 * @brief N边边界
 */
struct NsidedBoundary {
    std::vector<Edge> edges;
    std::vector<Face> adjacentFaces;
    std::vector<Vector3> normalConstraints;
    bool hasTangencyInfo = false;
    int numSides = 0;
};

/**
 * @brief 补面引擎
 * 
 * 自动填充N边形孔洞，支持多种曲面构造方法
 */
class PatchingEngine : public QObject {
    Q_OBJECT
    
public:
    explicit PatchingEngine(QObject* parent = nullptr);
    ~PatchingEngine();
    
    // N边曲面补面 (核心功能)
    PatchingResult patchNsided(const std::vector<Edge>& edges,
                               const PatchingOptions& options = PatchingOptions());
    
    PatchingResult patchNsided(const NsidedBoundary& boundary,
                               const PatchingOptions& options = PatchingOptions());
    
    // 孔洞填补
    PatchingResult fillHole(const Body& body, const std::vector<Edge>& boundaryEdges,
                            const PatchingOptions& options = PatchingOptions());
    
    // 间隙填补
    PatchingResult fillGap(const std::vector<Edge>& gapEdges, double gapTolerance,
                          const PatchingOptions& options = PatchingOptions());
    
    // 基于相邻面的曲率连续补面
    PatchingResult patchWithCurvatureContinuity(const std::vector<Edge>& edges,
                                                const std::vector<Face>& adjacentFaces,
                                                const PatchingOptions& options = PatchingOptions());
    
    // 批量补面
    std::vector<PatchingResult> batchPatch(const std::vector<NsidedBoundary>& boundaries,
                                          const PatchingOptions& options = PatchingOptions());
    
    // 自动选择最佳方法
    static PatchingMethod recommendMethod(const NsidedBoundary& boundary);
    
    // 质量评估
    static double evaluatePatchQuality(const Face& face, const NsidedBoundary& boundary);
    
    // 边界分析
    static NsidedBoundary analyzeBoundary(const std::vector<Edge>& edges,
                                         const std::vector<Face>& adjacentFaces = {});
    
    // 预检查
    static bool validateBoundary(const std::vector<Edge>& edges, QString* errorMsg = nullptr);
    
    // 设置进度回调
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }
    
signals:
    void patchingStarted();
    void patchingProgress(int percent, const QString& message);
    void patchingFinished(bool success);

private:
    ProgressCallback m_progressCallback = nullProgressCallback;
    
    void reportProgress(int percent, const QString& message);
    
    // 各种补面方法的实现
    PatchingResult patchWithGeomPlate(const NsidedBoundary& boundary, const PatchingOptions& options);
    PatchingResult patchWithCoons(const NsidedBoundary& boundary, const PatchingOptions& options);
    PatchingResult patchWithBSpline(const NsidedBoundary& boundary, const PatchingOptions& options);
};

} // namespace PS

#endif // PATCHINGENGINE_H
