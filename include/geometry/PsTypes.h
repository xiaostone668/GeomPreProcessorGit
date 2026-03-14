#ifndef PSTYPES_H
#define PSTYPES_H

#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include <QMetaType>
#include <QVector3D>

// Parasolid类型定义 - 在真实环境中会包含实际的头文件
// 这里定义简化版本用于演示
namespace PS {

// Parasolid实体类型枚举
enum class EntityType : int {
    UNKNOWN = 0,
    BODY = 1,
    SHELL = 2,
    FACE = 3,
    LOOP = 4,
    FIN = 5,
    EDGE = 6,
    VERTEX = 7,
    SURFACE = 8,
    CURVE = 9,
    TRANSF = 10,
    PART = 11,
    ASSEMBLY = 12,
    INSTANCE = 13
};

// Parasolid实体句柄 (模拟)
using PK_ENTITY_t = int;
using PK_BODY_t = int;
using PK_FACE_t = int;
using PK_EDGE_t = int;
using PK_VERTEX_t = int;
using PK_LOOP_t = int;
using PK_CURVE_t = int;
using PK_SURF_t = int;

// 特殊值
constexpr PK_ENTITY_t PK_ENTITY_null = 0;
constexpr int PK_LOGICAL_true = 1;
constexpr int PK_LOGICAL_false = 0;

// 布尔操作类型
enum class BooleanType {
    UNION = 1,
    SUBTRACTION = 2,
    INTERSECTION = 3
};

// 缝合选项
struct SewingOptions {
    double tolerance = 1e-6;
    double maxTolerance = 1e-3;
    bool allowNonManifold = false;
    bool createShells = true;
    bool createSolids = true;
};

// 检查结果类型
enum class CheckType : int {
    NarrowFace = 0,      // 狭小面
    SmallFace = 1,       // 微小面
    Spike = 2,           // 尖刺
    DuplicateFace = 3,   // 重复面
    IntersectingFace = 4,// 相交面
    RedundantVertex = 5, // 冗余点
    RedundantEdge = 6,   // 冗余边
    SmallEdge = 7,       // 微小边
    Gap = 8,             // 间隙
    Count = 9
};

// 问题严重程度
enum class Severity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3
};

// 问题修复状态
enum class FixStatus {
    Unfixed = 0,
    Fixing = 1,
    Fixed = 2,
    Failed = 3,
    Ignored = 4
};

// 向量3D
struct Vector3 {
    double x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
    
    QVector3D toQVector3D() const { return QVector3D(x, y, z); }
    static Vector3 fromQVector3D(const QVector3D& v) { return Vector3(v.x(), v.y(), v.z()); }
};

// 边界盒
struct BoundingBox {
    Vector3 min;
    Vector3 max;
    
    BoundingBox() {}
    BoundingBox(const Vector3& min, const Vector3& max) : min(min), max(max) {}
    
    double width() const { return max.x - min.x; }
    double height() const { return max.y - min.y; }
    double depth() const { return max.z - min.z; }
    Vector3 center() const { return Vector3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2); }
    double diagonal() const { return std::sqrt(width()*width() + height()*height() + depth()*depth()); }
};

// 变换矩阵 (4x3, 行主序)
struct Transform {
    double m[4][3];
    
    Transform() {
        // 单位矩阵
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0;
    }
};

// 进度回调
using ProgressCallback = std::function<void(int percent, const std::string& message)>;

// 空回调
inline void nullProgressCallback(int, const std::string&) {}

} // namespace PS

Q_DECLARE_METATYPE(PS::EntityType)
Q_DECLARE_METATYPE(PS::CheckType)
Q_DECLARE_METATYPE(PS::Severity)
Q_DECLARE_METATYPE(PS::FixStatus)

#endif // PSTYPES_H
