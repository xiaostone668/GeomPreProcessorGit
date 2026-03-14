#include "ui/IssueTableModel.h"
#include <QColor>
#include <QFont>

namespace PS {

IssueTableModel::IssueTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    // 默认启用所有类型
    for (int i = 0; i < static_cast<int>(CheckType::Count); i++) {
        m_enabledTypes.insert(static_cast<CheckType>(i));
    }
    
    // 默认启用所有严重级别
    m_enabledSeverities.insert(Severity::Info);
    m_enabledSeverities.insert(Severity::Warning);
    m_enabledSeverities.insert(Severity::Error);
    m_enabledSeverities.insert(Severity::Critical);
    
    // 默认启用所有状态
    m_enabledStatuses.insert(FixStatus::Unfixed);
    m_enabledStatuses.insert(FixStatus::Fixing);
    m_enabledStatuses.insert(FixStatus::Fixed);
    m_enabledStatuses.insert(FixStatus::Failed);
    m_enabledStatuses.insert(FixStatus::Ignored);
}

void IssueTableModel::setIssues(const QVector<GeometryIssue>& issues)
{
    beginResetModel();
    m_issues = issues;
    m_checkedIds.clear();
    rebuildFilter();
    endResetModel();
    
    emit issueSelectionChanged(selectedCount());
}

void IssueTableModel::addIssue(const GeometryIssue& issue)
{
    int newRow = m_issues.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_issues.append(issue);
    
    if (passesFilter(issue)) {
        m_filteredIndices.append(newRow);
    }
    
    endInsertRows();
}

void IssueTableModel::clearIssues()
{
    beginResetModel();
    m_issues.clear();
    m_filteredIndices.clear();
    m_checkedIds.clear();
    endResetModel();
    
    emit issueSelectionChanged(0);
}

void IssueTableModel::updateIssue(int row, const GeometryIssue& issue)
{
    if (row >= 0 && row < m_issues.size()) {
        m_issues[row] = issue;
        
        // 找到在筛选列表中的位置
        for (int i = 0; i < m_filteredIndices.size(); i++) {
            if (m_filteredIndices[i] == row) {
                emit dataChanged(index(i, 0), index(i, columnCount() - 1));
                break;
            }
        }
    }
}

void IssueTableModel::setFilterType(CheckType type, bool enabled)
{
    if (enabled) {
        m_enabledTypes.insert(type);
    } else {
        m_enabledTypes.remove(type);
    }
    rebuildFilter();
}

void IssueTableModel::setFilterSeverity(Severity severity, bool enabled)
{
    if (enabled) {
        m_enabledSeverities.insert(severity);
    } else {
        m_enabledSeverities.remove(severity);
    }
    rebuildFilter();
}

void IssueTableModel::setFilterStatus(FixStatus status, bool enabled)
{
    if (enabled) {
        m_enabledStatuses.insert(status);
    } else {
        m_enabledStatuses.remove(status);
    }
    rebuildFilter();
}

void IssueTableModel::setTextFilter(const QString& text)
{
    m_textFilter = text;
    rebuildFilter();
}

void IssueTableModel::clearFilters()
{
    m_enabledTypes.clear();
    for (int i = 0; i < static_cast<int>(CheckType::Count); i++) {
        m_enabledTypes.insert(static_cast<CheckType>(i));
    }
    
    m_enabledSeverities = {Severity::Info, Severity::Warning, Severity::Error, Severity::Critical};
    m_enabledStatuses = {FixStatus::Unfixed, FixStatus::Fixing, FixStatus::Fixed, FixStatus::Failed, FixStatus::Ignored};
    m_textFilter.clear();
    
    rebuildFilter();
}

const GeometryIssue* IssueTableModel::issueAt(int row) const
{
    if (row >= 0 && row < m_filteredIndices.size()) {
        return &m_issues[m_filteredIndices[row]];
    }
    return nullptr;
}

QVector<GeometryIssue> IssueTableModel::selectedIssues() const
{
    QVector<GeometryIssue> result;
    for (int i = 0; i < m_issues.size(); i++) {
        if (m_checkedIds.contains(m_issues[i].id)) {
            result.append(m_issues[i]);
        }
    }
    return result;
}

QVector<int> IssueTableModel::selectedIds() const
{
    return QVector<int>(m_checkedIds.begin(), m_checkedIds.end());
}

void IssueTableModel::setAllChecked(bool checked)
{
    beginResetModel();
    if (checked) {
        for (int i = 0; i < m_filteredIndices.size(); i++) {
            m_checkedIds.insert(m_issues[m_filteredIndices[i]].id);
        }
    } else {
        m_checkedIds.clear();
    }
    endResetModel();
    
    emit issueSelectionChanged(selectedCount());
}

void IssueTableModel::setChecked(int row, bool checked)
{
    if (row >= 0 && row < m_filteredIndices.size()) {
        int issueId = m_issues[m_filteredIndices[row]].id;
        
        if (checked) {
            m_checkedIds.insert(issueId);
        } else {
            m_checkedIds.remove(issueId);
        }
        
        emit dataChanged(index(row, Column::CheckBox), index(row, Column::CheckBox));
        emit issueSelectionChanged(selectedCount());
        emit issueChecked(issueId, checked);
    }
}

bool IssueTableModel::isChecked(int row) const
{
    if (row >= 0 && row < m_filteredIndices.size()) {
        return m_checkedIds.contains(m_issues[m_filteredIndices[row]].id);
    }
    return false;
}

int IssueTableModel::fixableCount() const
{
    int count = 0;
    for (const auto& issue : m_issues) {
        if (issue.isFixable() && m_checkedIds.contains(issue.id)) {
            count++;
        }
    }
    return count;
}

int IssueTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_filteredIndices.size();
}

int IssueTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(Column::Count);
}

QVariant IssueTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredIndices.size()) {
        return QVariant();
    }
    
    const GeometryIssue& issue = m_issues[m_filteredIndices[index.row()]];
    
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (static_cast<Column>(index.column())) {
            case Column::CheckBox:
                return QVariant();
            case Column::Id:
                return issue.id;
            case Column::Type:
                return typeToString(issue.type);
            case Column::Severity:
                return severityToString(issue.severity);
            case Column::Entity:
                return issue.entityType;
            case Column::Value:
                if (issue.value > 0) {
                    return QString::number(issue.value, 'e', 2);
                }
                return QVariant();
            case Column::Description:
                return issue.description;
            case Column::Status:
                return statusToString(issue.status);
            default:
                return QVariant();
        }
    }
    
    if (role == Qt::CheckStateRole && index.column() == Column::CheckBox) {
        return m_checkedIds.contains(issue.id) ? Qt::Checked : Qt::Unchecked;
    }
    
    if (role == Qt::BackgroundRole) {
        if (index.column() == Column::Severity) {
            return severityColor(issue.severity);
        }
        if (index.column() == Column::Status) {
            return statusColor(issue.status);
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == Column::Id || index.column() == Column::Value) {
            return Qt::AlignCenter;
        }
    }
    
    if (role == Qt::FontRole) {
        if (issue.severity == Severity::Critical || issue.severity == Severity::Error) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    
    return QVariant();
}

bool IssueTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == Column::CheckBox) {
        setChecked(index.row(), value.toInt() == Qt::Checked);
        return true;
    }
    return false;
}

QVariant IssueTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Column>(section)) {
            case Column::CheckBox: return QString();
            case Column::Id: return QObject::tr("ID");
            case Column::Type: return QObject::tr("类型");
            case Column::Severity: return QObject::tr("严重程度");
            case Column::Entity: return QObject::tr("实体");
            case Column::Value: return QObject::tr("数值");
            case Column::Description: return QObject::tr("描述");
            case Column::Status: return QObject::tr("状态");
            default: return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags IssueTableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    
    if (index.column() == Column::CheckBox) {
        flags |= Qt::ItemIsUserCheckable;
    }
    
    return flags;
}

bool IssueTableModel::passesFilter(const GeometryIssue& issue) const
{
    if (!m_enabledTypes.contains(issue.type)) return false;
    if (!m_enabledSeverities.contains(issue.severity)) return false;
    if (!m_enabledStatuses.contains(issue.status)) return false;
    
    if (!m_textFilter.isEmpty()) {
        if (!issue.description.contains(m_textFilter, Qt::CaseInsensitive) &&
            !issue.entityType.contains(m_textFilter, Qt::CaseInsensitive)) {
            return false;
        }
    }
    
    return true;
}

void IssueTableModel::rebuildFilter()
{
    beginResetModel();
    m_filteredIndices.clear();
    
    for (int i = 0; i < m_issues.size(); i++) {
        if (passesFilter(m_issues[i])) {
            m_filteredIndices.append(i);
        }
    }
    
    endResetModel();
}

QString IssueTableModel::typeToString(CheckType type) const
{
    switch (type) {
        case CheckType::NarrowFace: return QObject::tr("狭小面");
        case CheckType::SmallFace: return QObject::tr("微小面");
        case CheckType::Spike: return QObject::tr("尖刺");
        case CheckType::DuplicateFace: return QObject::tr("重复面");
        case CheckType::IntersectingFace: return QObject::tr("相交面");
        case CheckType::RedundantVertex: return QObject::tr("冗余点");
        case CheckType::RedundantEdge: return QObject::tr("冗余边");
        case CheckType::SmallEdge: return QObject::tr("微小边");
        case CheckType::Gap: return QObject::tr("间隙");
        default: return QObject::tr("未知");
    }
}

QString IssueTableModel::severityToString(Severity severity) const
{
    switch (severity) {
        case Severity::Info: return QObject::tr("信息");
        case Severity::Warning: return QObject::tr("警告");
        case Severity::Error: return QObject::tr("错误");
        case Severity::Critical: return QObject::tr("严重");
        default: return QObject::tr("未知");
    }
}

QString IssueTableModel::statusToString(FixStatus status) const
{
    switch (status) {
        case FixStatus::Unfixed: return QObject::tr("未修复");
        case FixStatus::Fixing: return QObject::tr("修复中");
        case FixStatus::Fixed: return QObject::tr("已修复");
        case FixStatus::Failed: return QObject::tr("失败");
        case FixStatus::Ignored: return QObject::tr("已忽略");
        default: return QObject::tr("未知");
    }
}

QColor IssueTableModel::severityColor(Severity severity) const
{
    switch (severity) {
        case Severity::Info: return QColor(200, 255, 200);
        case Severity::Warning: return QColor(255, 255, 200);
        case Severity::Error: return QColor(255, 200, 200);
        case Severity::Critical: return QColor(255, 100, 100);
        default: return QColor(255, 255, 255);
    }
}

QColor IssueTableModel::statusColor(FixStatus status) const
{
    switch (status) {
        case FixStatus::Unfixed: return QColor(255, 255, 255);
        case FixStatus::Fixing: return QColor(255, 255, 200);
        case FixStatus::Fixed: return QColor(200, 255, 200);
        case FixStatus::Failed: return QColor(255, 200, 200);
        case FixStatus::Ignored: return QColor(200, 200, 200);
        default: return QColor(255, 255, 255);
    }
}

} // namespace PS
