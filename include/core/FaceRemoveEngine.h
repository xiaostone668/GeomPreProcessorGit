#ifndef FACEREMOVEENGINE_H
#define FACEREMOVEENGINE_H

#include "geometry/PsTypes.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include <QObject>
#include <vector>
#include <memory>
#include <functional>

namespace PS {

/**
 * @brief 孔洞填补策略
 */
enum class HoleFillingStrategy {
    Auto,           // 自动选择
    Planar,         // 平面填充
    BestFit,        // 最佳拟合曲面
    Tangent,        // 切向连续曲面
    Patch,          // 使用补面引擎
    Bridge,         // 桥接
    LeaveOpen       // 保持开放
};

/**
 * @brief 删除面选项
 */
struct FaceRemoveOptions {
    HoleFillingStrategy fillingStrategy = HoleFillingStrategy::Auto;
    bool fillHoles = true;
    bool removeDependent = true;
    bool preserveTopology = true;
    bool checkManifold = true;
    double maxHoleSize = 0.0;
    double maxHoleAspectRatio = 10.0;
    bool simplifyBoundary = true;
    bool keepHistory = true;
};

/**
 * @brief 删除面结果
 */
struct FaceRemoveResult {
    bool success = false;
    Body resultBody;
    std::vector<Face> removedFaces;
    std::vector<Face> filledFaces;
    std::vector<std::vector<Edge>> remainingHoles;
    QString errorMessage;
};

/**
 * @brief 孔洞信息
 */
struct HoleInfo {
    std::vector<Edge> boundaryEdges;
    double area = 0.0;
    double maxDimension = 0.0;
    double aspectRatio = 1.0;
    bool canBeFilled = false;
    QString fillRecommendation;
};

/**
 * @brief 删除面引擎
 * 
 * 智能删除选定的面并自动填补产生的孔洞
 */
class FaceRemoveEngine : public QObject {
    Q_OBJECT
    
public:
    explicit FaceRemoveEngine(QObject* parent = nullptr);
    ~FaceRemoveEngine();
    
    // 删除单个面并填补
    FaceRemoveResult removeFace(const Body& body, const Face& face,
                               const FaceRemoveOptions& options = FaceRemoveOptions());
    
    // 批量删除面
    FaceRemoveResult removeFaces(const Body& body, const std::vector<Face>& faces,
                                const FaceRemoveOptions& options = FaceRemoveOptions());
    
    // 基于选择器删除
    FaceRemoveResult removeSelected(const Body& body,
                                   const std::function<bool(const Face&)>& selector,
                                   const FaceRemoveOptions& options = FaceRemoveOptions());
    
    // 智能删除
    FaceRemoveResult smartRemove(const Body& body, const Face& face, bool analyzeFirst = true);
    
    // 孔洞分析
    std::vector<HoleInfo> analyzeHoles(const Body& body);
    std::vector<HoleInfo> analyzeHolesAfterRemoval(const Body& body, const std::vector<Face>& facesToRemove);
    
    // 预检查
    bool canRemoveFace(const Body& body, const Face& face, QString* reason = nullptr);
    bool canRemoveFaces(const Body& body, const std::vector<Face>& faces, QString* reason = nullptr);
    
    // 依赖分析
    std::vector<Face> getDependentFaces(const Body& body, const Face& face);
    std::vector<Edge> getAffectedEdges(const Body& body, const Face& face);
    
    // 孔洞填补 (单独功能)
    Face fillHole(const Body& body, const HoleInfo& hole, const HoleFillingStrategy& strategy);
    
    // 边界简化
    std::vector<Edge> simplifyBoundary(const std::vector<Edge>& boundary, double tolerance = 1e-6);
    
    // 设置进度回调
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }
    
signals:
    void removalStarted();
    void removalProgress(int percent, const QString& message);
    void removalFinished(bool success);

private:
    ProgressCallback m_progressCallback = nullProgressCallback;
    
    void reportProgress(int percent, const QString& message);
};

} // namespace PS

#endif // FACEREMOVEENGINE_H
