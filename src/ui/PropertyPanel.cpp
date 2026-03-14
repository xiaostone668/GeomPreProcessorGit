#include "ui/PropertyPanel.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include "geometry/PsVertex.h"
#include <QTreeWidget>
#include <QLabel>
#include <QVBoxLayout>

namespace PS {

PropertyPanel::PropertyPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

PropertyPanel::~PropertyPanel() = default;

void PropertyPanel::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_titleLabel = new QLabel(tr("属性"), this);
    layout->addWidget(m_titleLabel);
    
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabel(tr("属性"));
    layout->addWidget(m_treeWidget, 1);
}

void PropertyPanel::clear()
{
    m_treeWidget->clear();
    m_titleLabel->setText(tr("属性"));
}

void PropertyPanel::setBody(const Body& body)
{
    clear();
    m_titleLabel->setText(tr("体属性"));
    
    addSeparator(tr("基本信息"));
    addProperty(tr("实体ID"), QString::number(body.entity()));
    addProperty(tr("类型"), body.isSolid() ? tr("实体") : tr("壳体"));
    
    addSeparator(tr("拓扑"));
    addProperty(tr("面数"), QString::number(body.faceCount()));
    addProperty(tr("边数"), QString::number(body.edgeCount()));
    addProperty(tr("顶点数"), QString::number(body.vertexCount()));
    
    addSeparator(tr("几何"));
    addProperty(tr("体积"), body.volume(), tr("mm3"));
    addProperty(tr("表面积"), body.surfaceArea(), tr("mm2"));
    
    if (body.isValid()) {
        BoundingBox box = body.boundingBox();
        addSeparator(tr("边界盒"));
        addProperty(tr("X范围"), QString("%1 - %2").arg(box.min.x).arg(box.max.x));
        addProperty(tr("Y范围"), QString("%1 - %2").arg(box.min.y).arg(box.max.y));
        addProperty(tr("Z范围"), QString("%1 - %2").arg(box.min.z).arg(box.max.z));
    }
}

void PropertyPanel::setFace(const Face& face)
{
    clear();
    m_titleLabel->setText(tr("面属性"));
    
    if (!face.isValid()) return;
    
    addSeparator(tr("基本信息"));
    addProperty(tr("实体ID"), QString::number(face.entity()));
    addProperty(tr("面数"), QString::number(face.edgeCount()));
    
    addSeparator(tr("几何"));
    addProperty(tr("面积"), face.area(), tr("mm2"));
    addProperty(tr("长宽比"), face.aspectRatio());
    
    QString surfaceType;
    if (face.isPlanar()) surfaceType = tr("平面");
    else if (face.isCylindrical()) surfaceType = tr("圆柱面");
    else if (face.isSpherical()) surfaceType = tr("球面");
    else if (face.isConical()) surfaceType = tr("圆锥面");
    else if (face.isBSpline()) surfaceType = tr("B样条曲面");
    else surfaceType = tr("其他");
    
    addProperty(tr("曲面类型"), surfaceType);
}

void PropertyPanel::setEdge(const Edge& edge)
{
    clear();
    m_titleLabel->setText(tr("边属性"));
    
    if (!edge.isValid()) return;
    
    addSeparator(tr("基本信息"));
    addProperty(tr("实体ID"), QString::number(edge.entity()));
    
    addSeparator(tr("几何"));
    addProperty(tr("长度"), edge.length(), tr("mm"));
    
    QString curveType;
    if (edge.isLine()) curveType = tr("直线");
    else if (edge.isArc()) curveType = tr("圆弧");
    else if (edge.isCircle()) curveType = tr("圆");
    else if (edge.isBSpline()) curveType = tr("B样条曲线");
    else curveType = tr("其他");
    
    addProperty(tr("曲线类型"), curveType);
}

void PropertyPanel::setVertex(const Vertex& vertex)
{
    clear();
    m_titleLabel->setText(tr("顶点属性"));
    
    if (!vertex.isValid()) return;
    
    addSeparator(tr("基本信息"));
    addProperty(tr("实体ID"), QString::number(vertex.entity()));
    
    Vector3 pos = vertex.position();
    addSeparator(tr("位置"));
    addProperty(tr("X"), pos.x);
    addProperty(tr("Y"), pos.y);
    addProperty(tr("Z"), pos.z);
    
    addProperty(tr("度数"), QString::number(vertex.degree()));
}

void PropertyPanel::setMultiple(int faceCount, int edgeCount, int vertexCount)
{
    clear();
    m_titleLabel->setText(tr("多项选择"));
    
    addProperty(tr("面"), QString::number(faceCount));
    addProperty(tr("边"), QString::number(edgeCount));
    addProperty(tr("顶点"), QString::number(vertexCount));
}

void PropertyPanel::addProperty(const QString& name, const QString& value)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, name);
    item->setText(1, value);
}

void PropertyPanel::addProperty(const QString& name, double value, const QString& unit)
{
    QString text;
    if (unit.isEmpty()) {
        text = QString::number(value, 'g', 6);
    } else {
        text = QString("%1 %2").arg(value, 0, 'g', 6).arg(unit);
    }
    addProperty(name, text);
}

void PropertyPanel::addSeparator(const QString& title)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);
    item->setText(0, title);
    item->setFlags(Qt::ItemIsEnabled);
    item->setBackground(0, QColor(200, 200, 200));
    item->setForeground(0, Qt::black);
}

} // namespace PS
