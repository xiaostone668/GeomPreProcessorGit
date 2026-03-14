#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include "geometry/PsVertex.h"
#include "core/PsKernel.h"
#include <QDebug>
#include <sstream>

namespace PS {

// ========== Body ==========

Body::Body()
    : m_entity(PK_ENTITY_null)
{
}

Body::Body(PK_BODY_t entity)
    : m_entity(entity)
{
}

Body::~Body()
{
}

Body Body::createSolid()
{
    // 在实际环境中调用: PK_BODY_create_solid(&body)
    return Body();
}

Body Body::fromFaces(const std::vector<Face>& faces)
{
    // 在实际环境中调用: PK_BODY_create_from_faces(count, face_ents, &body)
    return Body();
}

Body Body::copy(const Body& other)
{
    if (!other.isValid()) {
        return Body();
    }
    
    // 在实际环境中调用: PK_BODY_copy(other.m_entity, &new_body)
    return Body();
}

Body::Body(const Body& other)
    : m_entity(other.m_entity)
{
}

Body::Body(Body&& other) noexcept
    : m_entity(other.m_entity)
    , m_cachedFaces(std::move(other.m_cachedFaces))
    , m_cachedEdges(std::move(other.m_cachedEdges))
    , m_cachedVertices(std::move(other.m_cachedVertices))
    , m_facesCached(other.m_facesCached)
    , m_edgesCached(other.m_edgesCached)
    , m_verticesCached(other.m_verticesCached)
{
    other.m_entity = PK_ENTITY_null;
    other.m_facesCached = false;
    other.m_edgesCached = false;
    other.m_verticesCached = false;
}

Body& Body::operator=(const Body& other)
{
    if (this != &other) {
        m_entity = other.m_entity;
        m_facesCached = false;
        m_edgesCached = false;
        m_verticesCached = false;
    }
    return *this;
}

Body& Body::operator=(Body&& other) noexcept
{
    if (this != &other) {
        m_entity = other.m_entity;
        m_cachedFaces = std::move(other.m_cachedFaces);
        m_cachedEdges = std::move(other.m_cachedEdges);
        m_cachedVertices = std::move(other.m_cachedVertices);
        m_facesCached = other.m_facesCached;
        m_edgesCached = other.m_edgesCached;
        m_verticesCached = other.m_verticesCached;
        
        other.m_entity = PK_ENTITY_null;
        other.m_facesCached = false;
        other.m_edgesCached = false;
        other.m_verticesCached = false;
    }
    return *this;
}

bool Body::isValid() const
{
    return Kernel::instance().isValidEntity(m_entity);
}

void Body::setEntity(PK_BODY_t entity)
{
    m_entity = entity;
    m_facesCached = false;
    m_edgesCached = false;
    m_verticesCached = false;
}

Body::Type Body::type() const
{
    if (!isValid()) {
        return General;
    }
    
    // 在实际环境中调用: PK_BODY_ask_type(m_entity, &type)
    return Solid;
}

double Body::volume() const
{
    if (!isValid() || !isSolid()) {
        return 0.0;
    }
    
    // 在实际环境中调用: PK_BODY_ask_volume(m_entity, &volume)
    return 0.0;
}

double Body::surfaceArea() const
{
    if (!isValid()) {
        return 0.0;
    }
    
    // 在实际环境中调用: PK_BODY_ask_surface_area(m_entity, &area)
    return 0.0;
}

BoundingBox Body::boundingBox() const
{
    if (!isValid()) {
        return BoundingBox();
    }
    
    // 在实际环境中调用: PK_BODY_ask_extents(m_entity, &box)
    return BoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1));
}

Vector3 Body::centroid() const
{
    if (!isValid()) {
        return Vector3();
    }
    
    // 在实际环境中调用: PK_BODY_ask_centroid(m_entity, &centroid)
    return boundingBox().center();
}

std::vector<Face> Body::faces() const
{
    if (m_facesCached) {
        return m_cachedFaces;
    }
    
    m_cachedFaces.clear();
    if (!isValid()) {
        return m_cachedFaces;
    }
    
    // 在实际环境中调用: PK_BODY_ask_faces(m_entity, &count, &faces)
    m_facesCached = true;
    return m_cachedFaces;
}

std::vector<Edge> Body::edges() const
{
    if (m_edgesCached) {
        return m_cachedEdges;
    }
    
    m_cachedEdges.clear();
    if (!isValid()) {
        return m_cachedEdges;
    }
    
    // 在实际环境中调用: PK_BODY_ask_edges(m_entity, &count, &edges)
    m_edgesCached = true;
    return m_cachedEdges;
}

std::vector<Vertex> Body::vertices() const
{
    if (m_verticesCached) {
        return m_cachedVertices;
    }
    
    m_cachedVertices.clear();
    if (!isValid()) {
        return m_cachedVertices;
    }
    
    // 在实际环境中调用: PK_BODY_ask_vertices(m_entity, &count, &vertices)
    m_verticesCached = true;
    return m_cachedVertices;
}

int Body::faceCount() const
{
    return static_cast<int>(faces().size());
}

int Body::edgeCount() const
{
    return static_cast<int>(edges().size());
}

int Body::vertexCount() const
{
    return static_cast<int>(vertices().size());
}

bool Body::isClosed() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_is_closed(m_entity, &closed)
    return isSolid();
}

bool Body::isManifold() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_is_manifold(m_entity, &manifold)
    return true;
}

bool Body::isConvex() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_is_convex(m_entity, &convex)
    return false;
}

bool Body::isSelfIntersecting() const
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_is_self_intersecting(m_entity, &intersecting)
    return false;
}

void Body::transform(const Transform& transform)
{
    if (!isValid()) {
        return;
    }
    
    // 在实际环境中调用: PK_BODY_transform(m_entity, &transform)
    qDebug() << "Transforming body";
}

Body Body::transformed(const Transform& transform) const
{
    Body result = copy(*this);
    result.transform(transform);
    return result;
}

void Body::translate(const Vector3& offset)
{
    Transform t;
    t.m[3][0] = offset.x;
    t.m[3][1] = offset.y;
    t.m[3][2] = offset.z;
    transform(t);
}

void Body::rotate(const Vector3& axis, double angle)
{
    // 创建旋转矩阵
    Transform t;
    // 填充旋转矩阵...
    transform(t);
}

void Body::scale(double factor)
{
    Transform t;
    t.m[0][0] = factor;
    t.m[1][1] = factor;
    t.m[2][2] = factor;
    transform(t);
}

void Body::scale(const Vector3& factors)
{
    Transform t;
    t.m[0][0] = factors.x;
    t.m[1][1] = factors.y;
    t.m[2][2] = factors.z;
    transform(t);
}

Body Body::boolean(const Body& target, const Body& tool, BooleanType type)
{
    if (!target.isValid() || !tool.isValid()) {
        return Body();
    }
    
    // 在实际环境中调用: PK_BODY_boolean_2(target.m_entity, 1, &tool.m_entity, type, &result)
    return Body();
}

Body Body::unite(const Body& other) const
{
    return boolean(*this, other, BooleanType::UNION);
}

Body Body::subtract(const Body& other) const
{
    return boolean(*this, other, BooleanType::SUBTRACTION);
}

Body Body::intersect(const Body& other) const
{
    return boolean(*this, other, BooleanType::INTERSECTION);
}

void Body::sew(const SewingOptions& options)
{
    if (!isValid()) {
        return;
    }
    
    // 在实际环境中调用: PK_BODY_sew(m_entity, options.tolerance)
    qDebug() << "Sewing body with tolerance:" << options.tolerance;
}

Body Body::sewn(const SewingOptions& options) const
{
    Body result = copy(*this);
    result.sew(options);
    return result;
}

bool Body::fix()
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_fix(m_entity)
    return true;
}

bool Body::heal()
{
    if (!isValid()) {
        return false;
    }
    
    // 在实际环境中调用: PK_BODY_heal(m_entity)
    return true;
}

void Body::simplify()
{
    if (!isValid()) {
        return;
    }
    
    // 在实际环境中调用: PK_BODY_simplify(m_entity)
}

void Body::setMaterial(const std::string& name)
{
    // 设置材质属性
}

std::string Body::material() const
{
    return "";
}

void Body::setAttribute(const std::string& key, const std::string& value)
{
    // 设置自定义属性
}

std::string Body::attribute(const std::string& key) const
{
    return "";
}

bool Body::hasAttribute(const std::string& key) const
{
    return false;
}

size_t Body::geometryHash() const
{
    BoundingBox box = boundingBox();
    size_t h1 = std::hash<double>{}(box.min.x);
    size_t h2 = std::hash<double>{}(box.min.y);
    size_t h3 = std::hash<double>{}(box.min.z);
    size_t h4 = std::hash<double>{}(volume());
    
    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
}

std::string Body::toString() const
{
    std::ostringstream oss;
    oss << "Body {\n";
    oss << "  Entity: " << m_entity << "\n";
    oss << "  Type: " << static_cast<int>(type()) << "\n";
    oss << "  Valid: " << (isValid() ? "yes" : "no") << "\n";
    oss << "  Faces: " << faceCount() << "\n";
    oss << "  Edges: " << edgeCount() << "\n";
    oss << "  Vertices: " << vertexCount() << "\n";
    oss << "  Volume: " << volume() << "\n";
    oss << "  Area: " << surfaceArea() << "\n";
    oss << "  Solid: " << (isSolid() ? "yes" : "no") << "\n";
    oss << "  Closed: " << (isClosed() ? "yes" : "no") << "\n";
    oss << "  Manifold: " << (isManifold() ? "yes" : "no") << "\n";
    oss << "}";
    return oss.str();
}

} // namespace PS
