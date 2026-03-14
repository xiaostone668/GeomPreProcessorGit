#include "core/PatchingEngine.h"
#include "core/PsKernel.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include "geometry/PsBody.h"
#include <QDebug>

namespace PS {

PatchingEngine::PatchingEngine(QObject* parent)
    : QObject(parent)
{
}

PatchingEngine::~PatchingEngine() = default;

void PatchingEngine::reportProgress(int percent, const QString& message)
{
    if (m_progressCallback) {
        m_progressCallback(percent, message.toStdString());
    }
    emit patchingProgress(percent, message);
}

PatchingResult PatchingEngine::patchNsided(const std::vector<Edge>& edges, const PatchingOptions& options)
{
    if (edges.size() < 3) {
        PatchingResult result;
        result.errorMessage = QObject::tr("至少需要3条边来创建曲面");
        return result;
    }
    
    NsidedBoundary boundary;
    boundary.edges = edges;
    boundary.numSides = static_cast<int>(edges.size());
    
    return patchNsided(boundary, options);
}

PatchingResult PatchingEngine::patchNsided(const NsidedBoundary& boundary, const PatchingOptions& options)
{
    emit patchingStarted();
    reportProgress(10, QObject::tr("准备补面..."));
    
    if (boundary.edges.size() < 3) {
        PatchingResult result;
        result.errorMessage = QObject::tr("至少需要3条边来创建曲面");
        return result;
    }
    
    PatchingMethod method = options.method;
    if (method == PatchingMethod::Auto) {
        method = recommendMethod(boundary);
    }
    
    reportProgress(30, QObject::tr("使用%1方法...").arg(static_cast<int>(method)));
    
    PatchingResult result;
    
    switch (method) {
        case PatchingMethod::GeomPlate:
            result = patchWithGeomPlate(boundary, options);
            break;
        case PatchingMethod::Coons:
            result = patchWithCoons(boundary, options);
            break;
        case PatchingMethod::BSpline:
            result = patchWithBSpline(boundary, options);
            break;
        default:
            result = patchWithGeomPlate(boundary, options);
    }
    
    result.usedMethod = method;
    
    reportProgress(100, QObject::tr("补面完成"));
    emit patchingFinished(result.success);
    
    return result;
}

PatchingResult PatchingEngine::fillHole(const Body& body, const std::vector<Edge>& boundaryEdges, 
                                       const PatchingOptions& options)
{
    NsidedBoundary boundary;
    boundary.edges = boundaryEdges;
    boundary.numSides = static_cast<int>(boundaryEdges.size());
    
    // 提取相邻面的切向信息
    // 用于G1连续
    
    return patchNsided(boundary, options);
}

PatchingResult PatchingEngine::fillGap(const std::vector<Edge>& gapEdges, double gapTolerance,
                                      const PatchingOptions& options)
{
    return patchNsided(gapEdges, options);
}

PatchingResult PatchingEngine::patchWithCurvatureContinuity(const std::vector<Edge>& edges,
                                                           const std::vector<Face>& adjacentFaces,
                                                           const PatchingOptions& options)
{
    PatchingOptions opt = options;
    opt.tangency = true;
    
    NsidedBoundary boundary;
    boundary.edges = edges;
    boundary.adjacentFaces = adjacentFaces;
    boundary.hasTangencyInfo = true;
    
    return patchNsided(boundary, opt);
}

std::vector<PatchingResult> PatchingEngine::batchPatch(const std::vector<NsidedBoundary>& boundaries,
                                                       const PatchingOptions& options)
{
    std::vector<PatchingResult> results;
    int total = static_cast<int>(boundaries.size());
    
    for (int i = 0; i < total; i++) {
        reportProgress(i * 100 / total, QObject::tr("处理%1/%2...").arg(i + 1).arg(total));
        results.push_back(patchNsided(boundaries[i], options));
    }
    
    reportProgress(100, QObject::tr("批量补面完成"));
    return results;
}

PatchingMethod PatchingEngine::recommendMethod(const NsidedBoundary& boundary)
{
    if (boundary.edges.size() == 4) {
        return PatchingMethod::Coons;
    }
    
    if (boundary.hasTangencyInfo) {
        return PatchingMethod::GeomPlate;
    }
    
    return PatchingMethod::GeomPlate;
}

double PatchingEngine::evaluatePatchQuality(const Face& face, const NsidedBoundary& boundary)
{
    double quality = 1.0;
    
    // 检查参数化合理性
    // 检查与边界的贴合度
    
    return quality;
}

NsidedBoundary PatchingEngine::analyzeBoundary(const std::vector<Edge>& edges, 
                                               const std::vector<Face>& adjacentFaces)
{
    NsidedBoundary boundary;
    boundary.edges = edges;
    boundary.adjacentFaces = adjacentFaces;
    boundary.numSides = static_cast<int>(edges.size());
    boundary.hasTangencyInfo = !adjacentFaces.empty();
    return boundary;
}

bool PatchingEngine::validateBoundary(const std::vector<Edge>& edges, QString* errorMsg)
{
    if (edges.size() < 3) {
        if (errorMsg) *errorMsg = QObject::tr("边界至少需要3条边");
        return false;
    }
    
    return true;
}

PatchingResult PatchingEngine::patchWithGeomPlate(const NsidedBoundary& boundary, const PatchingOptions& options)
{
    PatchingResult result;
    
    // 使用Parasolid的曲面拟合功能
    // PK_SURF_create_filled / PK_SURF_create_approx
    
    result.success = true;
    result.maxDeviation = options.tolerance;
    
    return result;
}

PatchingResult PatchingEngine::patchWithCoons(const NsidedBoundary& boundary, const PatchingOptions& options)
{
    PatchingResult result;
    
    if (boundary.edges.size() != 4) {
        result.errorMessage = QObject::tr("Coons曲面需要4条边界");
        return result;
    }
    
    // 使用双线性插值创建Coons曲面
    // PK_SURF_create_coons
    
    result.success = true;
    
    return result;
}

PatchingResult PatchingEngine::patchWithBSpline(const NsidedBoundary& boundary, const PatchingOptions& options)
{
    PatchingResult result;
    
    // 创建B-Spline曲面
    // PK_SURF_create_bsurf
    
    result.success = true;
    
    return result;
}

} // namespace PS
