#ifndef BEAMEXTRACTENGINE_H
#define BEAMEXTRACTENGINE_H

#include "geometry/PsTypes.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QObject>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 梁截面类型
 */
enum class BeamProfileType {
    Circular,       // 圆形
    Rectangular,    // 矩形
    I_Section,      // I型
    T_Section,      // T型
    L_Section,      // L型
    C_Section,      // C型
    Hollow,         // 空心
    Custom          // 自定义
};

/**
 * @brief 梁截面
 */
struct BeamProfile {
    BeamProfileType type = BeamProfileType::Circular;
    double dimension1 = 0.0;
    double dimension2 = 0.0;
    double dimension3 = 0.0;
    double area = 0.0;
    double momentOfInertia = 0.0;
};

/**
 * @brief 梁段
 */
struct BeamSegment {
    std::vector<Vector3> spinePoints;
    double length = 0.0;
    std::vector<BeamProfile> profiles;
    std::vector<double> profileParams;
    Vector3 startPoint;
    Vector3 endPoint;
    Vector3 startTangent;
    Vector3 endTangent;
    bool isCurved = false;
    double maxCurvature = 0.0;
};

/**
 * @brief 梁信息
 */
struct BeamInfo {
    std::vector<BeamSegment> segments;
    BeamProfileType dominantProfile;
    double totalLength = 0.0;
    double totalVolume = 0.0;
    double avgCrossSectionArea = 0.0;
    int numSegments = 0;
    bool isTapered = false;
    bool isCurved = false;
};

/**
 * @brief 提取选项
 */
struct BeamExtractOptions {
    bool useSkeleton = true;
    bool useFeatureRecognition = true;
    double minBeamLength = 1e-3;
    double minCrossSectionArea = 1e-6;
    double maxCurvature = 1.0;
    bool detectBranches = true;
    bool detectJunctions = true;
    bool allowTapered = true;
    double taperThreshold = 0.1;
    bool mergeCollinear = true;
    double collinearTolerance = 1e-6;
};

/**
 * @brief 提取结果
 */
struct BeamExtractResult {
    bool success = false;
    std::vector<BeamInfo> beams;
    std::vector<Face> recognizedProfiles;
    std::vector<Edge> skeletonEdges;
    std::vector<Vector3> junctionPoints;
    QString errorMessage;
};

/**
 * @brief 截面信息
 */
struct CrossSectionInfo {
    Face face;
    Vector3 centerPoint;
    Vector3 normal;
    double area = 0.0;
    BeamProfileType detectedType;
    std::vector<double> dimensions;
    double circularity = 0.0;
    double rectangularity = 0.0;
};

/**
 * @brief 抽梁引擎
 * 
 * 从实体或壳体模型中提取梁结构
 */
class BeamExtractEngine : public QObject {
    Q_OBJECT
    
public:
    explicit BeamExtractEngine(QObject* parent = nullptr);
    ~BeamExtractEngine();
    
    // 从实体提取梁结构
    BeamExtractResult extractBeams(const Body& body, const BeamExtractOptions& options = BeamExtractOptions());
    
    // 从壳体提取 (薄壁梁)
    BeamExtractResult extractFromSheet(const Body& body, const BeamExtractOptions& options = BeamExtractOptions());
    
    // 基于中心线和截面提取
    BeamExtractResult extractFromSpineAndProfiles(const std::vector<Edge>& spine,
                                                  const std::vector<CrossSectionInfo>& crossSections);
    
    // 截面分析
    CrossSectionInfo analyzeCrossSection(const Face& face);
    std::vector<CrossSectionInfo> findCrossSections(const Body& body, const std::vector<Face>& candidates);
    
    // 骨架提取
    std::vector<Edge> extractSkeleton(const Body& body);
    std::vector<Edge> extractSkeletonMedialAxis(const Body& body);
    
    // 特征识别
    static BeamProfileType recognizeProfileType(const CrossSectionInfo& section);
    static std::vector<double> computeProfileDimensions(const CrossSectionInfo& section);
    
    // 梁几何生成
    Body generateBeamSolid(const BeamInfo& beam);
    Body generateBeamSheet(const BeamInfo& beam, double thickness);
    
    // 梁分析
    static double computeBeamVolume(const BeamInfo& beam);
    static double computeBeamLength(const BeamInfo& beam);
    static Vector3 computeCentroid(const BeamInfo& beam);
    
    // 批量处理
    std::vector<BeamExtractResult> batchExtract(const std::vector<Body>& bodies,
                                               const BeamExtractOptions& options = BeamExtractOptions());
    
    // 导出功能
    bool exportToBeamElements(const std::vector<BeamInfo>& beams, const QString& filePath);
    bool exportToCSV(const std::vector<BeamInfo>& beams, const QString& filePath);
    
    // 设置进度回调
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }
    
signals:
    void extractionStarted();
    void extractionProgress(int percent, const QString& message);
    void extractionFinished(bool success);

private:
    ProgressCallback m_progressCallback = nullProgressCallback;
    
    void reportProgress(int percent, const QString& message);
};

} // namespace PS

#endif // BEAMEXTRACTENGINE_H
