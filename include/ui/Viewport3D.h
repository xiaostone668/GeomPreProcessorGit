#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include "geometry/PsBody.h"
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QWheelEvent;
class QPaintEvent;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 3D视口
 * 
 * 使用Qt3D或OpenGL显示Parasolid几何体
 */
class Viewport3D : public QWidget {
    Q_OBJECT
    
public:
    enum ViewMode {
        Wireframe,
        Shaded,
        HiddenLine,
        WireframeOnShaded
    };
    
    explicit Viewport3D(QWidget* parent = nullptr);
    ~Viewport3D();
    
    void setBody(const std::shared_ptr<Body>& body);
    std::shared_ptr<Body> body() const { return m_body; }
    
    // 视图控制
    void viewFront();
    void viewTop();
    void viewRight();
    void viewLeft();
    void viewBottom();
    void viewBack();
    void viewIso();
    void viewFitAll();
    
    // 视图模式
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const { return m_viewMode; }
    
    // 选择
    void setSelectedFaces(const std::vector<Face>& faces);
    void setSelectedEdges(const std::vector<Edge>& edges);
    void setSelectedVertices(const std::vector<Vertex>& vertices);
    void clearSelection();
    
    std::vector<Face> selectedFaces() const { return m_selectedFaces; }
    std::vector<Edge> selectedEdges() const { return m_selectedEdges; }
    
signals:
    void selectionChanged();
    void faceSelected(const Face& face);
    void edgeSelected(const Edge& edge);
    void vertexSelected(const Vertex& vertex);
    
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void paintEvent(QPaintEvent* event) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    void setupScene();
    void updateCamera();
    void renderBody();
    void renderFaces();
    void renderEdges();
    void renderVertices();
    void renderSelection();
    
    std::shared_ptr<Body> m_body;
    std::vector<Face> m_selectedFaces;
    std::vector<Edge> m_selectedEdges;
    std::vector<Vertex> m_selectedVertices;
    
    ViewMode m_viewMode = Shaded;
    
    // 相机参数
    float m_cameraDistance = 10.0f;
    float m_cameraAzimuth = 45.0f;
    float m_cameraElevation = 30.0f;
    float m_cameraTarget[3] = {0, 0, 0};
    
    // 鼠标交互
    bool m_isDragging = false;
    QPoint m_lastMousePos;
    
    bool m_isInitialized = false;
};

} // namespace PS

#endif // VIEWPORT3D_H
