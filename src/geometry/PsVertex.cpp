#include "geometry/PsVertex.h"
#include "geometry/PsEdge.h"
#include "core/PsKernel.h"
#include <QDebug>

namespace PS {

Vertex::Vertex()
    : m_entity(PK_ENTITY_null)
{
}

Vertex::Vertex(PK_VERTEX_t entity)
    : m_entity(entity)
{
}

Vertex::~Vertex()
{
}

Vertex::Vertex(const Vertex& other)
    : m_entity(other.m_entity)
{
}

Vertex::Vertex(Vertex&& other) noexcept
    : m_entity(other.m_entity)
{
    other.m_entity = PK_ENTITY_null;
}

Vertex& Vertex::operator=(const Vertex& other)
{
    if (this != &other) {
        m_entity = other.m_entity;
    }
    return *this;
}

Vertex& Vertex::operator=(Vertex&& other) noexcept
{
    if (this != &other) {
        m_entity = other.m_entity;
        other.m_entity = PK_ENTITY_null;
    }
    return *this;
}

bool Vertex::isValid() const
{
    return Kernel::instance().isValidEntity(m_entity);
}

void Vertex::setEntity(PK_VERTEX_t entity)
{
    m_entity = entity;
}

Vector3 Vertex::position() const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_VERTEX_ask_point(m_entity, &point)
    // 这里返回模拟值
    return Vector3(0, 0, 0);
}

void Vertex::setPosition(const Vector3& pos)
{
    if (!isValid()) {
        return;
    }
    // 在实际环境中调用: PK_VERTEX_set_point(m_entity, &point)
    qDebug() << "Setting vertex position to:" << pos.x << pos.y << pos.z;
}

std::vector<std::shared_ptr<Edge>> Vertex::edges() const
{
    std::vector<std::shared_ptr<Edge>> result;
    if (!isValid()) {
        return result;
    }
    
    // 在实际环境中调用: PK_VERTEX_ask_edges(m_entity, &count, &edges)
    return result;
}

std::shared_ptr<Body> Vertex::body() const
{
    if (!isValid()) {
        return nullptr;
    }
    
    // 在实际环境中调用: PK_VERTEX_ask_body(m_entity, &body)
    return nullptr;
}

int Vertex::degree() const
{
    if (!isValid()) {
        return 0;
    }
    
    // 在实际环境中调用: PK_VERTEX_ask_edges(m_entity, &count, &edges)
    // 返回边的数量
    return edges().size();
}

} // namespace PS
