#include "geometry/PsEdge.h"
#include "geometry/PsFace.h"
#include "geometry/PsVertex.h"
#include "core/PsKernel.h"
#include <cmath>
#include <QDebug>

namespace PS {

Edge::Edge()
    : m_entity(PK_ENTITY_null)
{
}

Edge::Edge(PK_EDGE_t entity)
    : m_entity(entity)
{
}

Edge::~Edge()
{
}

Edge::Edge(const Edge& other)
    : m_entity(other.m_entity)
{
}

Edge::Edge(Edge&& other) noexcept
    : m_entity(other.m_entity)
{
    other.m_entity = PK_ENTITY_null;
}

Edge& Edge::operator=(const Edge& other)
{
    if (this != &other) {
        m_entity = other.m_entity;
    }
    return *this;
}

Edge& Edge::operator=(Edge&& other) noexcept
{
    if (this != &other) {
        m_entity = other.m_entity;
        other.m_entity = PK_ENTITY_null;
    }
    return *this;
}

bool Edge::isValid() const
{
    return Kernel::instance().isValidEntity(m_entity);
}

void Edge::setEntity(PK_EDGE_t entity)
{
    m_entity = entity;
}

double Edge::length() const
{
    if (!isValid()) {
        return 0.0;
    }
    
    // 在实际环境中调用: PK_EDGE_ask_length(m_entity, &length)
    return 0.0;
}

Vector3 Edge::startPoint() const
{
    if (!isValid()) {
        return Vector3();
    }
    
    Vertex start = startVertex();
    return start.position();
}

Vector3 Edge::endPoint() const
{
    if (!isValid()) {
        return Vector3();
    }
    
    Vertex end = endVertex();
    return end.position();
}

Vector3 Edge::pointAt(double parameter) const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_EDGE_eval(m_entity, parameter, point)
    return Vector3();
}

Vector3 Edge::tangentAt(double parameter) const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_EDGE_eval(m_entity, parameter, point, tangent)
    return Vector3(1, 0, 0);
}

bool Edge::isLine() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_EDGE_ask_curve(m_entity, &curve)
    // 然后检查曲线类型
    return true;
}

bool Edge::isArc() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Edge::isCircle() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

bool Edge::isBSpline() const
{
    if (!isValid()) {
        return false;
    }
    return false;
}

Vertex Edge::startVertex() const
{
    if (!isValid()) {
        return Vertex();
    }
    
    // 在实际环境中调用: PK_EDGE_ask_vertices(m_entity, &start, &end)
    return Vertex();
}

Vertex Edge::endVertex() const
{
    if (!isValid()) {
        return Vertex();
    }
    
    // 在实际环境中调用: PK_EDGE_ask_vertices(m_entity, &start, &end)
    return Vertex();
}

std::vector<Vertex> Edge::vertices() const
{
    std::vector<Vertex> result;
    result.push_back(startVertex());
    result.push_back(endVertex());
    return result;
}

std::vector<std::shared_ptr<Face>> Edge::faces() const
{
    std::vector<std::shared_ptr<Face>> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_EDGE_ask_faces(m_entity, &count, &faces)
    return result;
}

std::shared_ptr<Body> Edge::body() const
{
    if (!isValid()) {
        return nullptr;
    }
    
    // 在实际环境中调用: PK_EDGE_ask_body(m_entity, &body)
    return nullptr;
}

bool Edge::isG1Continuous() const
{
    // 简化实现
    return true;
}

bool Edge::isG2Continuous() const
{
    // 简化实现
    return true;
}

bool Edge::isReversed() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_EDGE_ask_orient(m_entity, &orient)
    return false;
}

} // namespace PS
