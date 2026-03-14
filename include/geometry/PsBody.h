#ifndef PSBODY_H
#define PSBODY_H

#include "PsTypes.h"
#include "PsFace.h"
#include "PsEdge.h"
#include "PsVertex.h"
#include <memory>
#include <vector>

namespace PS {

/**
 * @brief Parasolid实体(Body)封装类
 * 
 * Body是Parasolid中的顶层拓扑实体，可以是实体(Solid)、
 * 壳体(Shell)或线框(Wireframe)
 */
class Body {
public:
    // 体类型
    enum Type {
        Solid = 1,      // 实体
        Sheet = 2,      // 薄壳
        Wire = 3,       // 线框
        General = 4,    // 一般体
        Minimum = 5     // 最小体
    };
    
    // 构造/析构
    Body();
    explicit Body(PK_BODY_t entity);
    ~Body();
    
    // 从几何创建
    static Body createSolid();
    static Body fromFaces(const std::vector<Face>& faces);
    static Body copy(const Body& other);
    
    // 拷贝/移动
    Body(const Body& other);
    Body(Body&& other) noexcept;
    Body& operator=(const Body& other);
    Body& operator=(Body&& other) noexcept;
    
    // 有效性
    bool isValid() const;
    bool isNull() const { return m_entity == PK_ENTITY_null; }
    
    // 实体访问
    PK_BODY_t entity() const { return m_entity; }
    void setEntity(PK_BODY_t entity);
    
    // 体类型
    Type type() const;
    bool isSolid() const { return type() == Solid; }
    bool isSheet() const { return type() == Sheet; }
    bool isWire() const { return type() == Wire; }
    
    // 几何属性
    double volume() const;
    double surfaceArea() const;
    BoundingBox boundingBox() const;
    Vector3 centroid() const;
    
    // 拓扑访问
    std::vector<Face> faces() const;
    std::vector<Edge> edges() const;
    std::vector<Vertex> vertices() const;
    
    int faceCount() const;
    int edgeCount() const;
    int vertexCount() const;
    
    // 几何检查
    bool isClosed() const;
    bool isManifold() const;
    bool isConvex() const;
    bool isSelfIntersecting() const;
    
    // 变换
    void transform(const Transform& transform);
    Body transformed(const Transform& transform) const;
    void translate(const Vector3& offset);
    void rotate(const Vector3& axis, double angle);
    void scale(double factor);
    void scale(const Vector3& factors);
    
    // 布尔操作
    static Body boolean(const Body& target, const Body& tool, BooleanType type);
    Body unite(const Body& other) const;
    Body subtract(const Body& other) const;
    Body intersect(const Body& other) const;
    
    // 缝合
    void sew(const SewingOptions& options = SewingOptions());
    Body sewn(const SewingOptions& options = SewingOptions()) const;
    
    // 修复
    bool fix();
    bool heal();
    void simplify();
    
    // 材质属性
    void setMaterial(const std::string& name);
    std::string material() const;
    
    // 元数据
    void setAttribute(const std::string& key, const std::string& value);
    std::string attribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    
    // 比较
    bool operator==(const Body& other) const { return m_entity == other.m_entity; }
    bool operator!=(const Body& other) const { return !(*this == other); }
    bool operator<(const Body& other) const { return m_entity < other.m_entity; }
    
    // 几何哈希 (用于重复检测)
    size_t geometryHash() const;
    
    // 调试输出
    std::string toString() const;
    
private:
    PK_BODY_t m_entity = PK_ENTITY_null;
    mutable std::vector<Face> m_cachedFaces;
    mutable std::vector<Edge> m_cachedEdges;
    mutable std::vector<Vertex> m_cachedVertices;
    mutable bool m_facesCached = false;
    mutable bool m_edgesCached = false;
    mutable bool m_verticesCached = false;
};

} // namespace PS

#endif // PSBODY_H
