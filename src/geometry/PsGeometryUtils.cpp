#include "geometry/PsTypes.h"
#include "geometry/PsVertex.h"
#include "geometry/PsEdge.h"
#include <cmath>

namespace PS {

// 几何工具函数

double Distance(const Vector3& a, const Vector3& b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

Vector3 Normalize(const Vector3& v)
{
    double len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len > 0) {
        return Vector3(v.x/len, v.y/len, v.z/len);
    }
    return v;
}

Vector3 Cross(const Vector3& a, const Vector3& b)
{
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

double Dot(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double AngleBetween(const Vector3& a, const Vector3& b)
{
    double dot = Dot(Normalize(a), Normalize(b));
    if (dot < -1.0) dot = -1.0;
    if (dot > 1.0) dot = 1.0;
    return std::acos(dot) * 180.0 / M_PI;
}

bool IsParallel(const Vector3& a, const Vector3& b, double tolerance)
{
    double crossLen = std::sqrt(Cross(a, b).x * Cross(a, b).x +
                                 Cross(a, b).y * Cross(a, b).y +
                                 Cross(a, b).z * Cross(a, b).z);
    return crossLen < tolerance;
}

bool IsCollinear(const Vector3& a, const Vector3& b, const Vector3& c, double tolerance)
{
    Vector3 ab(b.x - a.x, b.y - a.y, b.z - a.z);
    Vector3 ac(c.x - a.x, c.y - a.y, c.z - a.z);
    return IsParallel(ab, ac, tolerance);
}

} // namespace PS
