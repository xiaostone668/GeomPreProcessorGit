#ifndef PSFACE_H
#define PSFACE_H

#include "PsTypes.h"
#include "PsEdge.h"
#include <memory>
#include <vector>

namespace PS {

class Body;
class Loop;

/**
 * @brief Parasolid面封装类
 */
class Face {
public:
    // 构造/析构
    Face();
    explicit Face(PK_FACE_t entity);
    ~Face();
    
    // 拷贝/移动
    Face(const Face& other);
    Face(Face&& other) noexcept;
    Face& operator=(const Face& other);
    Face& operator=(Face&& other) noexcept;
    
    // 有效性
    bool isValid() const;
    bool isNull() const { return m_entity == PK_ENTITY_null; }
    
    // 实体访问
    PK_FACE_t entity() const { return m_entity; }
    void setEntity(PK_FACE_t entity);
    
    // 几何属性
    double area() const;
    BoundingBox boundingBox() const;
    Vector3 normalAt(double u, double v) const;
    Vector3 pointAt(double u, double v) const;
    void uvBounds(double& uMin, double& uMax, double& vMin, double& vMax) const;
    
    // 长宽比 (用于狭小面检测)
    double aspectRatio() const;
    double maxDimension() const;
    double minDimension() const;
    
    // 表面类型
    bool isPlanar() const;
    bool isCylindrical() const;
    bool isSpherical() const;
    bool isConical() const;
    bool isToroidal() const;
    bool isBSpline() const;
    
    // 拓扑访问
    std::vector<Edge> edges() const;
    std::vector<Loop> loops() const;
    Loop outerLoop() const;
    std::vector<Loop> innerLoops() const;
    std::vector<Face> adjacentFaces() const;
    std::shared_ptr<Body> body() const;
    
    // 边数
    int edgeCount() const;
    
    // 面方向
    bool isReversed() const;
    
    // 检查点是否在面上
    bool containsPoint(const Vector3& point, double tolerance = 1e-6) const;
    
    // 检查是否为微小面/狭小面
    bool isSmall(double areaThreshold) const { return area() < areaThreshold; }
    bool isNarrow(double aspectThreshold) const { return aspectRatio() > aspectThreshold; }
    
    // 几何中心
    Vector3 center() const;
    
    // 比较
    bool operator==(const Face& other) const { return m_entity == other.m_entity; }
    bool operator!=(const Face& other) const { return !(*this == other); }
    bool operator<(const Face& other) const { return m_entity < other.m_entity; }
    
    // 哈希值 (用于重复面检测)
    size_t geometryHash() const;
    
private:
    PK_FACE_t m_entity = PK_ENTITY_null;
};

/**
 * @brief 环(Loop)封装类
 */
class Loop {
public:
    Loop();
    explicit Loop(PK_LOOP_t entity);
    ~Loop();
    
    bool isValid() const;
    PK_LOOP_t entity() const { return m_entity; }
    
    std::vector<Edge> edges() const;
    Face face() const;
    bool isOuter() const; // 是否为外环
    
private:
    PK_LOOP_t m_entity = PK_ENTITY_null;
};

} // namespace PS

#endif // PSFACE_H
