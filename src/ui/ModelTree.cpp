#include "ui/ModelTree.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"
#include "geometry/PsEdge.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>

namespace PS {

ModelTree::ModelTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderLabel(tr("模型结构"));
    setContextMenuPolicy(Qt::CustomContextMenu);
    
    connect(this, &QTreeWidget::itemClicked, this, &ModelTree::onItemClicked);
    connect(this, &QTreeWidget::itemDoubleClicked, this, &ModelTree::onItemDoubleClicked);
    connect(this, &QTreeWidget::customContextMenuRequested, this, &ModelTree::onContextMenu);
}

ModelTree::~ModelTree() = default;

void ModelTree::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
    buildTree();
}

void ModelTree::refresh()
{
    buildTree();
}

void ModelTree::setSelectedFaces(const std::vector<Face>& faces)
{
}

void ModelTree::setSelectedEdges(const std::vector<Edge>& edges)
{
}

void ModelTree::buildTree()
{
    clear();
    
    if (!m_body || m_body->isNull()) {
        return;
    }
    
    addBodyItem(invisibleRootItem());
}

void ModelTree::addBodyItem(QTreeWidgetItem* parent)
{
    QTreeWidgetItem* bodyItem = new QTreeWidgetItem(parent);
    bodyItem->setText(0, tr("Body (%1面, %2边, %3顶点)")
        .arg(m_body->faceCount())
        .arg(m_body->edgeCount())
        .arg(m_body->vertexCount()));
    bodyItem->setData(0, Qt::UserRole, QVariant::fromValue(EntityType::BODY));
    
    // 添加面节点
    auto faces = m_body->faces();
    for (size_t i = 0; i < faces.size(); i++) {
        addFaceItem(bodyItem, faces[i], static_cast<int>(i));
    }
}

void ModelTree::addFaceItem(QTreeWidgetItem* parent, const Face& face, int index)
{
    QTreeWidgetItem* faceItem = new QTreeWidgetItem(parent);
    faceItem->setText(0, tr("Face %1 (面积: %2)")
        .arg(index)
        .arg(face.area(), 0, 'e', 2));
    faceItem->setData(0, Qt::UserRole, QVariant::fromValue(EntityType::FACE));
    faceItem->setData(0, Qt::UserRole + 1, static_cast<qint64>(face.entity()));
}

void ModelTree::addEdgeItem(QTreeWidgetItem* parent, const Edge& edge, int index)
{
    QTreeWidgetItem* edgeItem = new QTreeWidgetItem(parent);
    edgeItem->setText(0, tr("Edge %1 (长度: %2)")
        .arg(index)
        .arg(edge.length(), 0, 'e', 2));
    edgeItem->setData(0, Qt::UserRole, QVariant::fromValue(EntityType::EDGE));
    edgeItem->setData(0, Qt::UserRole + 1, static_cast<qint64>(edge.entity()));
}

void ModelTree::addVertexItem(QTreeWidgetItem* parent, const Vertex& vertex, int index)
{
    QTreeWidgetItem* vertexItem = new QTreeWidgetItem(parent);
    vertexItem->setText(0, tr("Vertex %1").arg(index));
    vertexItem->setData(0, Qt::UserRole, QVariant::fromValue(EntityType::VERTEX));
    vertexItem->setData(0, Qt::UserRole + 1, static_cast<qint64>(vertex.entity()));
}

void ModelTree::onItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)
    
    if (!item) return;
    
    EntityType type = item->data(0, Qt::UserRole).value<EntityType>();
    
    switch (type) {
        case EntityType::BODY:
            emit bodySelected();
            break;
        case EntityType::FACE: {
            PK_ENTITY_t entity = static_cast<PK_ENTITY_t>(item->data(0, Qt::UserRole + 1).toLongLong());
            emit faceSelected(Face(entity));
            break;
        }
        case EntityType::EDGE: {
            PK_ENTITY_t entity = static_cast<PK_ENTITY_t>(item->data(0, Qt::UserRole + 1).toLongLong());
            emit edgeSelected(Edge(entity));
            break;
        }
        default:
            break;
    }
}

void ModelTree::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    onItemClicked(item, column);
}

void ModelTree::onContextMenu(const QPoint& pos)
{
    QMenu menu(this);
    menu.addAction(tr("显示"));
    menu.addAction(tr("隐藏"));
    menu.addSeparator();
    menu.addAction(tr("选择"));
    menu.addAction(tr("属性"));
    menu.exec(mapToGlobal(pos));
}

} // namespace PS
