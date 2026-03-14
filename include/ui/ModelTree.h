#ifndef MODELTREE_H
#define MODELTREE_H

#include "geometry/PsBody.h"
#include <QTreeWidget>
#include <memory>

namespace PS {

class Body;

/**
 * @brief 模型树
 * 
 * 显示Parasolid体的拓扑结构
 */
class ModelTree : public QTreeWidget {
    Q_OBJECT
    
public:
    explicit ModelTree(QWidget* parent = nullptr);
    ~ModelTree();
    
    void setBody(const std::shared_ptr<Body>& body);
    void refresh();
    
    void setSelectedFaces(const std::vector<Face>& faces);
    void setSelectedEdges(const std::vector<Edge>& edges);
    
signals:
    void faceSelected(const Face& face);
    void edgeSelected(const Edge& edge);
    void vertexSelected(const Vertex& vertex);
    void bodySelected();
    
private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenu(const QPoint& pos);
    
private:
    void buildTree();
    void addBodyItem(QTreeWidgetItem* parent);
    void addFaceItem(QTreeWidgetItem* parent, const Face& face, int index);
    void addEdgeItem(QTreeWidgetItem* parent, const Edge& edge, int index);
    void addVertexItem(QTreeWidgetItem* parent, const Vertex& vertex, int index);
    
    std::shared_ptr<Body> m_body;
};

} // namespace PS

#endif // MODELTREE_H
