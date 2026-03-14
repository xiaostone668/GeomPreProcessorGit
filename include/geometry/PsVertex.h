#ifndef PSVERTEX_H
#define PSVERTEX_H

#include "PsTypes.h"
#include <memory>

namespace PS {

class Edge;
class Body;

/**
 * @brief Parasolid顶点封装类
 */
class Vertex {
public:
    // 构造/析构
    Vertex();
    explicit Vertex(PK_VERTEX_t entity);
    ~Vertex();
    
    // 拷贝/移动
    Vertex(const Vertex& other);
    Vertex(Vertex&& other) noexcept;
    Vertex& operator=(const Vertex& other);
    Vertex& operator=(Vertex&& other) noexcept;
    
    // 有效性
    bool isValid() const;
    bool isNull() const { return m_entity == PK_ENTITY_null; }
    
    // 实体访问
    PK_VERTEX_t entity() const { return m_entity; }
    void setEntity(PK_VERTEX_t entity);
    
    // 几何属性
    Vector3 position() const;
    void setPosition(const Vector3& pos);
    
    // 拓扑访问
    std::vector<std::shared_ptr<Edge>> edges() const;
    std::shared_ptr<Body> body() const;
    
    // 度数 (连接的边数)
    int degree() const;
    
    // 比较
    bool operator==(const Vertex& other) const { return m_entity == other.m_entity; }
    bool operator!=(const Vertex& other) const { return !(*this == other); }
    bool operator<(const Vertex& other) const { return m_entity < other.m_entity; }
    
private:
    PK_VERTEX_t m_entity = PK_ENTITY_null;
};

} // namespace PS

#endif // PSVERTEX_H
