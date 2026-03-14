#include "geometry/PsFace.h"
#include "geometry/PsBody.h"
#include "geometry/PsEdge.h"
#include "core/PsKernel.h"
#include <algorithm>
#include <QDebug>

namespace PS {

// ========== Face ==========

Face::Face()
    : m_entity(PK_ENTITY_null)
{
}

Face::Face(PK_FACE_t entity)
    : m_entity(entity)
{
}

Face::~Face()
{
}

Face::Face(const Face& other)
    : m_entity(other.m_entity)
{
}

Face::Face(Face&& other) noexcept
    : m_entity(other.m_entity)
{
    other.m_entity = PK_ENTITY_null;
}

Face& Face::operator=(const Face& other)
{
    if (this != &other) {
        m_entity = other.m_entity;
    }
    return *this;
}

Face& Face::operator=(Face&& other) noexcept
{
    if (this != &other) {
        m_entity = other.m_entity;
        other.m_entity = PK_ENTITY_null;
    }
    return *this;
}

bool Face::isValid() const
{
    return Kernel::instance().isValidEntity(m_entity);
}

void Face::setEntity(PK_FACE_t entity)
{
    m_entity = entity;
}

double Face::area() const
{
    if (!isValid()) {
        return 0.0;
    }
    
    // 在实际环境中调用: PK_FACE_ask_area(m_entity, &area)
    return 0.0;
}

BoundingBox Face::boundingBox() const
{
    if (!isValid()) {
        return BoundingBox();
    }
    
    // 在实际环境中调用: PK_FACE_ask_box(m_entity, &box)
    return BoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1));
}

Vector3 Face::normalAt(double u, double v) const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_FACE_eval(m_entity, u, v, point, normal)
    return Vector3(0, 0, 1);
}

Vector3 Face::pointAt(double u, double v) const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_FACE_eval(m_entity, u, v, point)
    return Vector3();
}

void Face::uvBounds(double& uMin, double& uMax, double& vMin, double& vMax) const
{
    if (!isValid()) {
        uMin = uMax = vMin = vMax = 0.0;
        return;
    }
    
    // 在实际环境中调用: PK_FACE_ask_uvbox(m_entity, &u_min, &u_max, &v_min, &v_max)
    uMin = vMin = 0.0;
    uMax = vMax = 1.0;
}

double Face::aspectRatio() const
{
    BoundingBox box = boundingBox();
    double dims[3] = {box.width(), box.height(), box.depth()};
    std::sort(dims, dims + 3);
    
    if (dims[1] > 1e-10) {
        return dims[2] / dims[1];
    }
    return 1.0;
}

double Face::maxDimension() const
{
    BoundingBox box = boundingBox();
    return std::max({box.width(), box.height(), box.depth()});
}

double Face::minDimension() const
{
    BoundingBox box = boundingBox();
    double dims[3] = {box.width(), box.height(), box.depth()};
    std::sort(dims, dims + 3);
    return dims[0];
}

bool Face::isPlanar() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_FACE_ask_surface(m_entity, &surface)
    // 然后检查表面类型
    return true;
}

bool Face::isCylindrical() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Face::isSpherical() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Face::isConical() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Face::isToroidal() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Face::isBSpline() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

std::vector<Edge> Face::edges() const
{
    std::vector<Edge> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_FACE_ask_edges(m_entity, &count, &edges)
    return result;
}

std::vector<Loop> Face::loops() const
{
    std::vector<Loop> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_FACE_ask_loops(m_entity, &count, &loops)
    return result;
}

Loop Face::outerLoop() const
{
    std::vector<Loop> allLoops = loops();
    for (const auto& loop : allLoops) {
        if (loop.isOuter()) {
            return loop;
        }
    }
    return Loop();
}

std::vector<Loop> Face::innerLoops() const
{
    std::vector<Loop> result;
    std::vector<Loop> allLoops = loops();
    for (const auto& loop : allLoops) {
        if (!loop.isOuter()) {
            result.push_back(loop);
        }
    }
    return result;
}

std::vector<Face> Face::adjacentFaces() const
{
    std::vector<Face> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_FACE_ask_adj_faces(m_entity, &count, &faces)
    return result;
}

std::shared_ptr<Body> Face::body() const
{
    if (!isValid()) {
        return nullptr;
    }
    
    // 在实际环境中调用: PK_FACE_ask_body(m_entity, &body)
    return nullptr;
}

int Face::edgeCount() const
{
    return edges().size();
}

bool Face::isReversed() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_FACE_ask_orient(m_entity, &orient)
    return false;
}

bool Face::containsPoint(const Vector3& point, double tolerance) const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_FACE_contain_point(m_entity, point, tolerance)
    return false;
}

Vector3 Face::center() const
{
    BoundingBox box = boundingBox();
    return box.center();
}

size_t Face::geometryHash() const
{
    // 基于几何特征生成哈希值
    Vector3 c = center();
    size_t h1 = std::hash<double>{}(c.x);
    size_t h2 = std::hash<double>{}(c.y);
    size_t h3 = std::hash<double>{}(c.z);
    size_t h4 = std::hash<double>{}(area());
    
    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
}

// ========== Loop ==========

Loop::Loop()
    : m_entity(PK_ENTITY_null)
{
}

Loop::Loop(PK_LOOP_t entity)
    : m_entity(entity)
{
}

Loop::~Loop()
{
}

bool Loop::isValid() const
{
    return Kernel::instance().isValidEntity(m_entity);
}

std::vector<Edge> Loop::edges() const
{
    std::vector<Edge> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_LOOP_ask_edges(m_entity, &count, &edges)
    return result;
}

Face Loop::face() const
{
    if (!isValid()) {
        return Face();
    }
    
    // 在实际环境中调用: PK_LOOP_ask_face(m_entity, &face)
    return Face();
}

bool Loop::isOuter() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_LOOP_is_outer(m_entity, &is_outer)
    return true;
}

} // namespace PS
