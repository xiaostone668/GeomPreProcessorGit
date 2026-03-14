#include "core/BeamExtractEngine.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QDebug>
#include <cmath>

namespace PS {

BeamExtractEngine::BeamExtractEngine(QObject* parent)
    : QObject(parent)
{
}

BeamExtractEngine::~BeamExtractEngine() = default;

void BeamExtractEngine::reportProgress(int percent, const QString& message)
{
    if (m_progressCallback) {
        m_progressCallback(percent, message.toStdString());
    }
    emit extractionProgress(percent, message);
}

BeamExtractResult BeamExtractEngine::extractBeams(const Body& body, const BeamExtractOptions& options)
{
    emit extractionStarted();
    reportProgress(0, QObject::tr("开始提取梁结构..."));
    
    BeamExtractResult result;
    
    if (!body.isValid()) {
        result.errorMessage = QObject::tr("无效输入");
        emit extractionFinished(false);
        return result;
    }
    
    reportProgress(10, QObject::tr("识别候选截面..."));
    
    // 查找候选截面
    std::vector<Face> candidates;
    for (const auto& face : body.faces()) {
        // 检查是否可能是截面
        if (face.area() > options.minCrossSectionArea) {
            candidates.push_back(face);
        }
    }
    
    reportProgress(30, QObject::tr("分析截面特征..."));
    
    std::vector<CrossSectionInfo> crossSections;
    for (const auto& candidate : candidates) {
        auto info = analyzeCrossSection(candidate);
        if (info.area > options.minCrossSectionArea) {
            crossSections.push_back(info);
        }
    }
    
    reportProgress(50, QObject::tr("提取骨架..."));
    
    // 提取骨架
    auto skeleton = extractSkeleton(body);
    
    reportProgress(70, QObject::tr("构建梁段..."));
    
    // 构建梁信息
    for (const auto& edge : skeleton) {
        BeamInfo beam;
        BeamSegment segment;
        segment.spinePoints = {edge.startPoint(), edge.endPoint()};
        segment.length = edge.length();
        segment.startPoint = edge.startPoint();
        segment.endPoint = edge.endPoint();
        
        beam.segments.push_back(segment);
        beam.totalLength = segment.length;
        beam.numSegments = 1;
        beam.dominantProfile = BeamProfileType::Circular;
        beam.isTapered = false;
        beam.isCurved = false;
        beam.totalVolume = 0.0;
        beam.avgCrossSectionArea = 0.0;
        
        result.beams.push_back(beam);
    }
    
    result.skeletonEdges = skeleton;
    result.recognizedProfiles = candidates;
    result.success = !result.beams.empty();
    
    reportProgress(100, QObject::tr("提取完成"));
    emit extractionFinished(result.success);
    
    return result;
}

BeamExtractResult BeamExtractEngine::extractFromSheet(const Body& body, const BeamExtractOptions& options)
{
    // 处理薄壁壳体
    return extractBeams(body, options);
}

BeamExtractResult BeamExtractEngine::extractFromSpineAndProfiles(const std::vector<Edge>& spine,
                                                                 const std::vector<CrossSectionInfo>& crossSections)
{
    BeamExtractResult result;
    
    // 基于中心线和截面构建梁
    
    return result;
}

CrossSectionInfo BeamExtractEngine::analyzeCrossSection(const Face& face)
{
    CrossSectionInfo info;
    info.face = face;
    
    if (!face.isValid()) {
        return info;
    }
    
    // 计算中心点
    info.centerPoint = face.center();
    info.area = face.area();
    info.normal = face.normalAt(0.5, 0.5);
    info.detectedType = recognizeProfileType(info);
    info.dimensions = computeProfileDimensions(info);
    
    return info;
}

std::vector<CrossSectionInfo> BeamExtractEngine::findCrossSections(const Body& body, const std::vector<Face>& candidates)
{
    std::vector<CrossSectionInfo> sections;
    
    for (const auto& face : candidates) {
        auto info = analyzeCrossSection(face);
        sections.push_back(info);
    }
    
    return sections;
}

std::vector<Edge> BeamExtractEngine::extractSkeleton(const Body& body)
{
    std::vector<Edge> skeleton;
    
    if (!body.isValid()) {
        return skeleton;
    }
    
    // 提取中心线
    // 使用Parasolid的中轴变换或其他方法
    
    // 简化：返回所有边
    skeleton = body.edges();
    
    return skeleton;
}

std::vector<Edge> BeamExtractEngine::extractSkeletonMedialAxis(const Body& body)
{
    // 中轴变换提取骨架
    return extractSkeleton(body);
}

BeamProfileType BeamExtractEngine::recognizeProfileType(const CrossSectionInfo& section)
{
    // 基于几何特征识别截面类型
    
    if (section.circularity > 0.9) {
        return BeamProfileType::Circular;
    } else if (section.rectangularity > 0.9) {
        return BeamProfileType::Rectangular;
    }
    
    return BeamProfileType::Custom;
}

std::vector<double> BeamExtractEngine::computeProfileDimensions(const CrossSectionInfo& section)
{
    std::vector<double> dims;
    
    BoundingBox box = section.face.boundingBox();
    
    dims.push_back(box.width());
    dims.push_back(box.height());
    dims.push_back(box.depth());
    
    return dims;
}

Body BeamExtractEngine::generateBeamSolid(const BeamInfo& beam)
{
    if (beam.segments.empty()) {
        return Body();
    }
    
    // 使用扫掠创建梁实体
    // PK_FACE_make_sweep
    
    return Body();
}

Body BeamExtractEngine::generateBeamSheet(const BeamInfo& beam, double thickness)
{
    // 创建薄壁梁 (壳体)
    return Body();
}

double BeamExtractEngine::computeBeamVolume(const BeamInfo& beam)
{
    double volume = 0.0;
    
    for (const auto& segment : beam.segments) {
        double avgArea = 0.0;
        for (const auto& profile : segment.profiles) {
            avgArea += profile.area;
        }
        if (!segment.profiles.empty()) {
            avgArea /= segment.profiles.size();
        }
        volume += avgArea * segment.length;
    }
    
    return volume;
}

double BeamExtractEngine::computeBeamLength(const BeamInfo& beam)
{
    double length = 0.0;
    
    for (const auto& segment : beam.segments) {
        length += segment.length;
    }
    
    return length;
}

Vector3 BeamExtractEngine::computeCentroid(const BeamInfo& beam)
{
    Vector3 centroid(0, 0, 0);
    double totalLength = 0.0;
    
    for (const auto& segment : beam.segments) {
        Vector3 mid((segment.startPoint.x + segment.endPoint.x) / 2,
                    (segment.startPoint.y + segment.endPoint.y) / 2,
                    (segment.startPoint.z + segment.endPoint.z) / 2);
        
        centroid.x += mid.x * segment.length;
        centroid.y += mid.y * segment.length;
        centroid.z += mid.z * segment.length;
        totalLength += segment.length;
    }
    
    if (totalLength > 0.0) {
        centroid.x /= totalLength;
        centroid.y /= totalLength;
        centroid.z /= totalLength;
    }
    
    return centroid;
}

std::vector<BeamExtractResult> BeamExtractEngine::batchExtract(const std::vector<Body>& bodies,
                                                               const BeamExtractOptions& options)
{
    std::vector<BeamExtractResult> results;
    
    for (size_t i = 0; i < bodies.size(); i++) {
        reportProgress(static_cast<int>(i * 100 / bodies.size()),
                      QObject::tr("处理%1/%2...").arg(i + 1).arg(bodies.size()));
        results.push_back(extractBeams(bodies[i], options));
    }
    
    return results;
}

bool BeamExtractEngine::exportToBeamElements(const std::vector<BeamInfo>& beams, const QString& filePath)
{
    // 导出为有限元格式 (NASTRAN等)
    return false;
}

bool BeamExtractEngine::exportToCSV(const std::vector<BeamInfo>& beams, const QString& filePath)
{
    // 导出为CSV
    return false;
}

} // namespace PS
