#include "ui/Viewport3D.h"
#include "geometry/PsBody.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QDebug>

namespace PS {

Viewport3D::Viewport3D(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(400, 300);
}

Viewport3D::~Viewport3D() = default;

void Viewport3D::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
    update();
}

void Viewport3D::viewFront()
{
    m_cameraAzimuth = 90.0f;
    m_cameraElevation = 0.0f;
    update();
}

void Viewport3D::viewTop()
{
    m_cameraAzimuth = 0.0f;
    m_cameraElevation = 90.0f;
    update();
}

void Viewport3D::viewRight()
{
    m_cameraAzimuth = 0.0f;
    m_cameraElevation = 0.0f;
    update();
}

void Viewport3D::viewLeft()
{
    m_cameraAzimuth = 180.0f;
    m_cameraElevation = 0.0f;
    update();
}

void Viewport3D::viewBottom()
{
    m_cameraAzimuth = 0.0f;
    m_cameraElevation = -90.0f;
    update();
}

void Viewport3D::viewBack()
{
    m_cameraAzimuth = -90.0f;
    m_cameraElevation = 0.0f;
    update();
}

void Viewport3D::viewIso()
{
    m_cameraAzimuth = 45.0f;
    m_cameraElevation = 30.0f;
    update();
}

void Viewport3D::viewFitAll()
{
    // 计算边界并调整相机
    if (m_body && !m_body->isNull()) {
        BoundingBox box = m_body->boundingBox();
        m_cameraTarget[0] = box.center().x;
        m_cameraTarget[1] = box.center().y;
        m_cameraTarget[2] = box.center().z;
        m_cameraDistance = box.diagonal() * 1.5f;
    }
    update();
}

void Viewport3D::setViewMode(ViewMode mode)
{
    m_viewMode = mode;
    update();
}

void Viewport3D::setSelectedFaces(const std::vector<Face>& faces)
{
    m_selectedFaces = faces;
    update();
}

void Viewport3D::setSelectedEdges(const std::vector<Edge>& edges)
{
    m_selectedEdges = edges;
    update();
}

void Viewport3D::setSelectedVertices(const std::vector<Vertex>& vertices)
{
    m_selectedVertices = vertices;
    update();
}

void Viewport3D::clearSelection()
{
    m_selectedFaces.clear();
    m_selectedEdges.clear();
    m_selectedVertices.clear();
    update();
}

void Viewport3D::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));
    
    // 绘制标题
    painter.setPen(Qt::white);
    painter.drawText(10, 20, tr("3D视口 (简化的预览)"));
    
    if (m_body && !m_body->isNull()) {
        painter.drawText(10, 40, tr("面: %1, 边: %2, 顶点: %3")
            .arg(m_body->faceCount())
            .arg(m_body->edgeCount())
            .arg(m_body->vertexCount()));
    } else {
        painter.drawText(10, 40, tr("无几何体"));
    }
    
    // 绘制简单的坐标轴
    int centerX = width() / 2;
    int centerY = height() / 2;
    int axisLength = 50;
    
    painter.setPen(Qt::red);
    painter.drawLine(centerX, centerY, centerX + axisLength, centerY);
    painter.drawText(centerX + axisLength + 5, centerY, "X");
    
    painter.setPen(Qt::green);
    painter.drawLine(centerX, centerY, centerX, centerY - axisLength);
    painter.drawText(centerX + 5, centerY - axisLength - 5, "Y");
}

void Viewport3D::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
    }
}

void Viewport3D::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        
        m_cameraAzimuth += delta.x() * 0.5f;
        m_cameraElevation += delta.y() * 0.5f;
        
        // 限制仰角
        if (m_cameraElevation > 89.0f) m_cameraElevation = 89.0f;
        if (m_cameraElevation < -89.0f) m_cameraElevation = -89.0f;
        
        m_lastMousePos = event->pos();
        update();
    }
}

void Viewport3D::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
}

void Viewport3D::wheelEvent(QWheelEvent* event)
{
    m_cameraDistance *= (event->angleDelta().y() > 0) ? 0.9f : 1.1f;
    if (m_cameraDistance < 0.1f) m_cameraDistance = 0.1f;
    update();
}

void Viewport3D::initializeGL()
{
}

void Viewport3D::resizeGL(int width, int height)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
}

void Viewport3D::paintGL()
{
}

void Viewport3D::renderBody()
{
}

void Viewport3D::renderFaces()
{
}

void Viewport3D::renderEdges()
{
}

void Viewport3D::renderVertices()
{
}

void Viewport3D::renderSelection()
{
}

} // namespace PS
