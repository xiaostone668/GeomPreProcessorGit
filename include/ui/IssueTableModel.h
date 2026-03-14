#ifndef ISSUETABLEMODEL_H
#define ISSUETABLEMODEL_H

#include "core/CheckRepairEngine.h"
#include <QAbstractTableModel>
#include <QVector>
#include <QSet>

namespace PS {

/**
 * @brief 问题列表数据模型
 * 
 * 用于在QTableView中显示检查结果
 */
class IssueTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    enum Column {
        CheckBox = 0,      // 复选框
        Id = 1,            // ID
        Type = 2,          // 类型
        Severity = 3,      // 严重程度
        Entity = 4,        // 实体
        Value = 5,         // 量化值
        Description = 6,   // 描述
        Status = 7,        // 修复状态
        Count
    };
    
    explicit IssueTableModel(QObject* parent = nullptr);
    
    // 数据操作
    void setIssues(const QVector<GeometryIssue>& issues);
    void addIssue(const GeometryIssue& issue);
    void clearIssues();
    void updateIssue(int row, const GeometryIssue& issue);
    
    // 筛选
    void setFilterType(CheckType type, bool enabled);
    void setFilterSeverity(Severity severity, bool enabled);
    void setFilterStatus(FixStatus status, bool enabled);
    void setTextFilter(const QString& text);
    void clearFilters();
    
    // 获取数据
    const GeometryIssue* issueAt(int row) const;
    QVector<GeometryIssue> selectedIssues() const;
    QVector<int> selectedIds() const;
    
    // 选择操作
    void setAllChecked(bool checked);
    void setChecked(int row, bool checked);
    bool isChecked(int row) const;
    
    // 统计
    int totalCount() const { return m_issues.size(); }
    int filteredCount() const { return m_filteredIndices.size(); }
    int selectedCount() const { return m_checkedIds.size(); }
    int fixableCount() const;
    
    // QAbstractTableModel接口
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    
signals:
    void issueSelectionChanged(int selectedCount);
    void issueChecked(int issueId, bool checked);
    
private:
    QVector<GeometryIssue> m_issues;
    QVector<int> m_filteredIndices;  // 筛选后的索引映射
    QSet<int> m_checkedIds;          // 选中的ID集合
    
    // 筛选条件
    QSet<CheckType> m_enabledTypes;
    QSet<Severity> m_enabledSeverities;
    QSet<FixStatus> m_enabledStatuses;
    QString m_textFilter;
    
    bool passesFilter(const GeometryIssue& issue) const;
    void rebuildFilter();
    
    // 辅助函数
    QString typeToString(CheckType type) const;
    QString severityToString(Severity severity) const;
    QString statusToString(FixStatus status) const;
    QColor severityColor(Severity severity) const;
    QColor statusColor(FixStatus status) const;
};

} // namespace PS

#endif // ISSUETABLEMODEL_H
