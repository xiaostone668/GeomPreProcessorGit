#ifndef PSEDGE_H
#define PSEDGE_H

#include "PsTypes.h"
#include "PsVertex.h"
#include <memory>
#include <vector>

namespace PS {

class Face;
class Body;

/**
 * @brief Parasolid边封装类
 */
class Edge {
public:
    // 构造/析构
    Edge();
    explicit Edge(PK_EDGE_t entity);
    ~Edge();
    
    // 拷贝/移动
    Edge(const Edge& other);
    Edge(Edge&& other) noexcept;
    Edge& operator=(const Edge& other);
    Edge& operator=(Edge&& other) noexcept;
    
    // 有效性
    bool isValid() const;
    bool isNull() const { return m_entity == PK_ENTITY_null; }
    
    // 实体访问
    PK_EDGE_t entity() const { return m_entity; }
    void setEntity(PK_EDGE_t entity);
    
    // 几何属性
    double length() const;
    Vector3 startPoint() const;
    Vector3 endPoint() const;
    Vector3 pointAt(double parameter) const; // parameter: 0-1
    Vector3 tangentAt(double parameter) const;
    
    // 曲线类型
    bool isLine() const;
    bool isArc() const;
    bool isCircle() const;
    bool isBSpline() const;
    
    // 拓扑访问
    Vertex startVertex() const;
    Vertex endVertex() const;
    std::vector<Vertex> vertices() const;
    std::vector<std::shared_ptr<Face>> faces() const;
    std::shared_ptr<Body> body() const;
    
    // 几何连续性
    bool isG1Continuous() const;
    bool isG2Continuous() const;
    
    // 边方向
    bool isReversed() const;
    
    // 检查是否为微小边
    bool isSmall(double threshold) const { return length() < threshold; }
    
    // 比较
    bool operator==(const Edge& other) const { return m_entity == other.m_entity; }
    bool operator!=(const Edge& other) const { return !(*this == other); }
    bool operator<(const Edge& other) const { return m_entity < other.m_entity; }
    
private:
    PK_EDGE_t m_entity = PK_ENTITY_null;
};

} // namespace PS

#endif // PSEDGE_H
