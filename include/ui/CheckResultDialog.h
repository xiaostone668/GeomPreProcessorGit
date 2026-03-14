#ifndef CHECKRESULTDIALOG_H
#define CHECKRESULTDIALOG_H

#include "core/CheckRepairEngine.h"
#include "ui/IssueTableModel.h"
#include <QDialog>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QTableView;
class QPushButton;
class QComboBox;
class QLineEdit;
class QLabel;
class QProgressBar;
class QGroupBox;
class QCheckBox;
class QSplitter;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 检查结果对话框
 * 
 * 显示检查结果，支持筛选、选择和修复操作
 */
class CheckResultDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CheckResultDialog(QWidget* parent = nullptr);
    ~CheckResultDialog();
    
    // 设置数据
    void setBody(const std::shared_ptr<Body>& body);
    void setIssues(const std::vector<GeometryIssue>& issues);
    void setStatistics(const CheckStatistics& stats);
    
    // 获取结果
    const CheckStatistics& statistics() const { return m_statistics; }
    std::vector<GeometryIssue> fixedIssues() const;
    
    // 运行检查
    void runCheck();
    
public slots:
    void onCheckFinished();
    void onRepairFinished();
    void onIssueSelectionChanged();
    
private slots:
    void onFilterChanged();
    void onTextFilterChanged(const QString& text);
    void onSelectAll();
    void onSelectNone();
    void onSelectFixable();
    void onFixSelected();
    void onFixAll();
    void onViewIssue();
    void onExportReport();
    void onFilterButtonClicked();
    void onCheckTypeFilterChanged(int index);
    void onSeverityFilterChanged(int index);
    void onStatusFilterChanged(int index);
    
private:
    void setupUI();
    void setupConnections();
    void updateStatisticsDisplay();
    void updateButtonStates();
    void showProgress(const QString& message);
    void hideProgress();
    
    // UI元素
    QTableView* m_tableView = nullptr;
    IssueTableModel* m_model = nullptr;
    
    QPushButton* m_fixSelectedButton = nullptr;
    QPushButton* m_fixAllButton = nullptr;
    QPushButton* m_viewButton = nullptr;
    QPushButton* m_exportButton = nullptr;
    QPushButton* m_closeButton = nullptr;
    
    QComboBox* m_typeFilter = nullptr;
    QComboBox* m_severityFilter = nullptr;
    QComboBox* m_statusFilter = nullptr;
    QLineEdit* m_textFilter = nullptr;
    
    QLabel* m_totalLabel = nullptr;
    QLabel* m_criticalLabel = nullptr;
    QLabel* m_errorLabel = nullptr;
    QLabel* m_warningLabel = nullptr;
    QLabel* m_infoLabel = nullptr;
    QLabel* m_fixableLabel = nullptr;
    QLabel* m_selectedLabel = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_progressLabel = nullptr;
    
    QGroupBox* m_filterGroup = nullptr;
    QCheckBox* m_autoConfirmCheck = nullptr;
    
    // 数据
    std::shared_ptr<Body> m_body;
    std::shared_ptr<CheckRepairEngine> m_engine;
    CheckStatistics m_statistics;
    
    bool m_isChecking = false;
    bool m_isRepairing = false;
};

} // namespace PS

#endif // CHECKRESULTDIALOG_H
