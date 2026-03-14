#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include "geometry/PsTypes.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QLabel;
QT_END_NAMESPACE

namespace PS {

class Body;
class Face;
class Edge;
class Vertex;

/**
 * @brief 属性面板
 * 
 * 显示选中对象的属性
 */
class PropertyPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit PropertyPanel(QWidget* parent = nullptr);
    ~PropertyPanel();
    
    void clear();
    void setBody(const Body& body);
    void setFace(const Face& face);
    void setEdge(const Edge& edge);
    void setVertex(const Vertex& vertex);
    void setMultiple(int faceCount, int edgeCount, int vertexCount);
    
private:
    void setupUI();
    void addProperty(const QString& name, const QString& value);
    void addProperty(const QString& name, double value, const QString& unit = QString());
    void addSeparator(const QString& title);
    
    QTreeWidget* m_treeWidget = nullptr;
    QLabel* m_titleLabel = nullptr;
};

} // namespace PS

#endif // PROPERTYPANEL_H
