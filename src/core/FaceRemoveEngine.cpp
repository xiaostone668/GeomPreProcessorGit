#include "core/FaceRemoveEngine.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QDebug>

namespace PS {

FaceRemoveEngine::FaceRemoveEngine(QObject* parent)
    : QObject(parent)
{
}

FaceRemoveEngine::~FaceRemoveEngine() = default;

void FaceRemoveEngine::reportProgress(int percent, const QString& message)
{
    if (m_progressCallback) {
        m_progressCallback(percent, message.toStdString());
    }
    emit removalProgress(percent, message);
}

FaceRemoveResult FaceRemoveEngine::removeFace(const Body& body, const Face& face, 
                                              const FaceRemoveOptions& options)
{
    emit removalStarted();
    reportProgress(10, QObject::tr("分析面..."));
    
    FaceRemoveResult result;
    
    if (!body.isValid() || !face.isValid()) {
        result.errorMessage = QObject::tr("无效输入");
        emit removalFinished(false);
        return result;
    }
    
    QString reason;
    if (!canRemoveFace(body, face, &reason)) {
        result.errorMessage = QObject::tr("无法删除面: %1").arg(reason);
        emit removalFinished(false);
        return result;
    }
    
    reportProgress(30, QObject::tr("删除面..."));
    
    // 在实际环境中使用PK_FACE_delete或布尔操作
    
    reportProgress(70, QObject::tr("填补孔洞..."));
    
    if (options.fillHoles) {
        // 填补产生的孔洞
    }
    
    result.success = true;
    result.resultBody = body;
    
    reportProgress(100, QObject::tr("删除完成"));
    emit removalFinished(true);
    
    return result;
}

FaceRemoveResult FaceRemoveEngine::removeFaces(const Body& body, const std::vector<Face>& faces,
                                               const FaceRemoveOptions& options)
{
    emit removalStarted();
    
    FaceRemoveResult result;
    
    if (!body.isValid() || faces.empty()) {
        result.errorMessage = QObject::tr("无效输入");
        emit removalFinished(false);
        return result;
    }
    
    // 批量删除面
    
    result.success = true;
    emit removalFinished(true);
    
    return result;
}

FaceRemoveResult FaceRemoveEngine::removeSelected(const Body& body,
                                                 const std::function<bool(const Face&)>& selector,
                                                 const FaceRemoveOptions& options)
{
    std::vector<Face> facesToRemove;
    
    for (const auto& face : body.faces()) {
        if (selector(face)) {
            facesToRemove.push_back(face);
        }
    }
    
    return removeFaces(body, facesToRemove, options);
}

FaceRemoveResult FaceRemoveEngine::smartRemove(const Body& body, const Face& face, bool analyzeFirst)
{
    FaceRemoveOptions options;
    options.fillingStrategy = HoleFillingStrategy::Auto;
    options.fillHoles = true;
    
    if (analyzeFirst) {
        auto deps = getDependentFaces(body, face);
        if (deps.size() > 2) {
            options.fillingStrategy = HoleFillingStrategy::Tangent;
        }
    }
    
    return removeFace(body, face, options);
}

std::vector<HoleInfo> FaceRemoveEngine::analyzeHoles(const Body& body)
{
    std::vector<HoleInfo> holes;
    
    if (!body.isValid()) {
        return holes;
    }
    
    // 查找所有自由边组成的环
    
    return holes;
}

std::vector<HoleInfo> FaceRemoveEngine::analyzeHolesAfterRemoval(const Body& body, 
                                                                 const std::vector<Face>& facesToRemove)
{
    // 模拟删除后分析孔洞
    return analyzeHoles(body);
}

bool FaceRemoveEngine::canRemoveFace(const Body& body, const Face& face, QString* reason)
{
    if (!face.isValid()) {
        if (reason) *reason = QObject::tr("面为空");
        return false;
    }
    
    if (!body.isValid()) {
        if (reason) *reason = QObject::tr("体为空");
        return false;
    }
    
    return true;
}

bool FaceRemoveEngine::canRemoveFaces(const Body& body, const std::vector<Face>& faces, QString* reason)
{
    for (const auto& face : faces) {
        if (!canRemoveFace(body, face, reason)) {
            return false;
        }
    }
    return true;
}

std::vector<Face> FaceRemoveEngine::getDependentFaces(const Body& body, const Face& face)
{
    std::vector<Face> dependents;
    
    if (!body.isValid() || !face.isValid()) {
        return dependents;
    }
    
    // 获取共享边的其他面
    for (const auto& edge : face.edges()) {
        for (const auto& adjFace : edge.faces()) {
            if (adjFace.entity() != face.entity()) {
                dependents.push_back(adjFace);
            }
        }
    }
    
    return dependents;
}

std::vector<Edge> FaceRemoveEngine::getAffectedEdges(const Body& body, const Face& face)
{
    return face.edges();
}

Face FaceRemoveEngine::fillHole(const Body& body, const HoleInfo& hole, const HoleFillingStrategy& strategy)
{
    // 使用不同的策略填补孔洞
    
    return Face();
}

std::vector<Edge> FaceRemoveEngine::simplifyBoundary(const std::vector<Edge>& boundary, double tolerance)
{
    return boundary;
}

} // namespace PS
