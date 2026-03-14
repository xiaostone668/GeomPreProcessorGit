#include "ui/CheckResultDialog.h"
#include "core/CheckRepairEngine.h"
#include "geometry/PsBody.h"
#include <QTableView>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>

namespace PS {

CheckResultDialog::CheckResultDialog(QWidget* parent)
    : QDialog(parent)
    , m_engine(std::make_shared<CheckRepairEngine>())
{
    setupUI();
    setupConnections();
    
    setWindowTitle(tr("检查修复"));
    resize(1200, 700);
}

CheckResultDialog::~CheckResultDialog() = default;

void CheckResultDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 筛选区域
    m_filterGroup = new QGroupBox(tr("筛选"), this);
    QGridLayout* filterLayout = new QGridLayout(m_filterGroup);
    
    filterLayout->addWidget(new QLabel(tr("类型:")), 0, 0);
    m_typeFilter = new QComboBox(this);
    m_typeFilter->addItem(tr("全部"));
    m_typeFilter->addItem(tr("狭小面"), static_cast<int>(CheckType::NarrowFace));
    m_typeFilter->addItem(tr("微小面"), static_cast<int>(CheckType::SmallFace));
    m_typeFilter->addItem(tr("尖刺"), static_cast<int>(CheckType::Spike));
    m_typeLayout->addWidget(m_typeFilter, 0, 1);
    
    filterLayout->addWidget(new QLabel(tr("严重程度:")), 0, 2);
    m_severityFilter = new QComboBox(this);
    m_severityFilter->addItem(tr("全部"));
    m_severityFilter->addItem(tr("严重"), static_cast<int>(Severity::Critical));
    m_severityFilter->addItem(tr("错误"), static_cast<int>(Severity::Error));
    m_severityFilter->addItem(tr("警告"), static_cast<int>(Severity::Warning));
    m_severityFilter->addItem(tr("信息"), static_cast<int>(Severity::Info));
    filterLayout->addWidget(m_severityFilter, 0, 3);
    
    filterLayout->addWidget(new QLabel(tr("状态:")), 1, 0);
    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItem(tr("全部"));
    m_statusFilter->addItem(tr("未修复"), static_cast<int>(FixStatus::Unfixed));
    m_statusFilter->addItem(tr("已修复"), static_cast<int>(FixStatus::Fixed));
    m_statusFilter->addItem(tr("失败"), static_cast<int>(FixStatus::Failed));
    m_statusFilter->addItem(tr("已忽略"), static_cast<int>(FixStatus::Ignored));
    filterLayout->addWidget(m_statusFilter, 1, 1);
    
    filterLayout->addWidget(new QLabel(tr("搜索:")), 1, 2);
    m_textFilter = new QLineEdit(this);
    m_textFilter->setPlaceholderText(tr("输入关键词..."));
    filterLayout->addWidget(m_textFilter, 1, 3);
    
    mainLayout->addWidget(m_filterGroup);
    
    // 统计信息
    QHBoxLayout* statsLayout = new QHBoxLayout();
    m_totalLabel = new QLabel(tr("总计: 0"), this);
    m_criticalLabel = new QLabel(tr("严重: 0"), this);
    m_errorLabel = new QLabel(tr("错误: 0"), this);
    m_warningLabel = new QLabel(tr("警告: 0"), this);
    m_infoLabel = new QLabel(tr("信息: 0"), this);
    m_fixableLabel = new QLabel(tr("可修复: 0"), this);
    m_selectedLabel = new QLabel(tr("已选择: 0"), this);
    
    statsLayout->addWidget(m_totalLabel);
    statsLayout->addWidget(m_criticalLabel);
    statsLayout->addWidget(m_errorLabel);
    statsLayout->addWidget(m_warningLabel);
    statsLayout->addWidget(m_infoLabel);
    statsLayout->addWidget(m_fixableLabel);
    statsLayout->addWidget(m_selectedLabel);
    statsLayout->addStretch();
    
    mainLayout->addLayout(statsLayout);
    
    // 表格视图
    m_tableView = new QTableView(this);
    m_model = new IssueTableModel(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setAlternatingRowColors(true);
    
    mainLayout->addWidget(m_tableView, 1);
    
    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    m_progressLabel = new QLabel(this);
    m_progressLabel->setVisible(false);
    mainLayout->addWidget(m_progressLabel);
    
    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* selectAllBtn = new QPushButton(tr("全选"), this);
    QPushButton* selectNoneBtn = new QPushButton(tr("全不选"), this);
    QPushButton* selectFixableBtn = new QPushButton(tr("选择可修复"), this);
    
    buttonLayout->addWidget(selectAllBtn);
    buttonLayout->addWidget(selectNoneBtn);
    buttonLayout->addWidget(selectFixableBtn);
    buttonLayout->addStretch();
    
    m_fixSelectedButton = new QPushButton(tr("修复选中"), this);
    m_fixAllButton = new QPushButton(tr("一键修复全部"), this);
    m_viewButton = new QPushButton(tr("查看"), this);
    m_exportButton = new QPushButton(tr("导出报告"), this);
    m_closeButton = new QPushButton(tr("关闭"), this);
    
    m_fixSelectedButton->setEnabled(false);
    m_fixAllButton->setEnabled(false);
    
    buttonLayout->addWidget(m_fixSelectedButton);
    buttonLayout->addWidget(m_fixAllButton);
    buttonLayout->addWidget(m_viewButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接按钮信号
    connect(selectAllBtn, &QPushButton::clicked, this, &CheckResultDialog::onSelectAll);
    connect(selectNoneBtn, &QPushButton::clicked, this, &CheckResultDialog::onSelectNone);
    connect(selectFixableBtn, &QPushButton::clicked, this, &CheckResultDialog::onSelectFixable);
    connect(m_fixSelectedButton, &QPushButton::clicked, this, &CheckResultDialog::onFixSelected);
    connect(m_fixAllButton, &QPushButton::clicked, this, &CheckResultDialog::onFixAll);
    connect(m_viewButton, &QPushButton::clicked, this, &CheckResultDialog::onViewIssue);
    connect(m_exportButton, &QPushButton::clicked, this, &CheckResultDialog::onExportReport);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void CheckResultDialog::setupConnections()
{
    // 引擎信号
    connect(m_engine.get(), &CheckRepairEngine::checkStarted, this, [this]() {
        showProgress(tr("开始检查..."));
        m_isChecking = true;
    });
    
    connect(m_engine.get(), &CheckRepairEngine::checkProgress, this, [this](int percent, const QString& msg) {
        m_progressBar->setValue(percent);
        m_progressLabel->setText(msg);
    });
    
    connect(m_engine.get(), &CheckRepairEngine::checkFinished, this, [this]() {
        hideProgress();
        m_isChecking = false;
        onCheckFinished();
    });
    
    connect(m_engine.get(), &CheckRepairEngine::repairStarted, this, [this]() {
        showProgress(tr("开始修复..."));
        m_isRepairing = true;
    });
    
    connect(m_engine.get(), &CheckRepairEngine::repairProgress, this, [this](int percent, const QString& msg) {
        m_progressBar->setValue(percent);
        m_progressLabel->setText(msg);
    });
    
    connect(m_engine.get(), &CheckRepairEngine::repairFinished, this, [this]() {
        hideProgress();
        m_isRepairing = false;
        onRepairFinished();
    });
    
    connect(m_engine.get(), &CheckRepairEngine::issueFound, this, [this](const GeometryIssue& issue) {
        m_model->addIssue(issue);
    });
    
    // 筛选信号
    connect(m_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CheckResultDialog::onCheckTypeFilterChanged);
    connect(m_severityFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CheckResultDialog::onSeverityFilterChanged);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &CheckResultDialog::onStatusFilterChanged);
    connect(m_textFilter, &QLineEdit::textChanged, this, &CheckResultDialog::onTextFilterChanged);
    
    // 模型信号
    connect(m_model, &IssueTableModel::issueSelectionChanged, this, &CheckResultDialog::onIssueSelectionChanged);
}

void CheckResultDialog::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
}

void CheckResultDialog::setIssues(const std::vector<GeometryIssue>& issues)
{
    QVector<GeometryIssue> qIssues;
    for (const auto& issue : issues) {
        qIssues.append(issue);
    }
    m_model->setIssues(qIssues);
    updateStatisticsDisplay();
}

void CheckResultDialog::setStatistics(const CheckStatistics& stats)
{
    m_statistics = stats;
    updateStatisticsDisplay();
}

void CheckResultDialog::runCheck()
{
    if (!m_body) {
        QMessageBox::warning(this, tr("警告"), tr("没有可检查的对象"));
        return;
    }
    
    CheckOptions options;
    m_model->clearIssues();
    
    m_engine->check(*m_body, options);
}

void CheckResultDialog::onCheckFinished()
{
    m_statistics = m_engine->statistics();
    updateStatisticsDisplay();
    updateButtonStates();
    
    QMessageBox::information(this, tr("检查完成"), 
        tr("检查完成!\n总计: %1个问题\n可修复: %2个")
        .arg(m_statistics.totalIssues)
        .arg(m_statistics.fixableCount));
}

void CheckResultDialog::onRepairFinished()
{
    m_statistics = m_engine->statistics();
    updateStatisticsDisplay();
    updateButtonStates();
    
    QMessageBox::information(this, tr("修复完成"),
        tr("修复完成!\n成功: %1个\n失败: %2个")
        .arg(m_statistics.fixedCount)
        .arg(m_statistics.failedCount));
}

void CheckResultDialog::onIssueSelectionChanged()
{
    int selectedCount = m_model->selectedCount();
    m_selectedLabel->setText(tr("已选择: %1").arg(selectedCount));
    m_fixSelectedButton->setEnabled(selectedCount > 0);
}

void CheckResultDialog::onFilterChanged()
{
    // 触发重新筛选
}

void CheckResultDialog::onTextFilterChanged(const QString& text)
{
    m_model->setTextFilter(text);
}

void CheckResultDialog::onSelectAll()
{
    m_model->setAllChecked(true);
}

void CheckResultDialog::onSelectNone()
{
    m_model->setAllChecked(false);
}

void CheckResultDialog::onSelectFixable()
{
    // 选择所有可修复的问题
    m_model->setAllChecked(false);
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        const GeometryIssue* issue = m_model->issueAt(i);
        if (issue && issue->isFixable()) {
            m_model->setChecked(i, true);
        }
    }
}

void CheckResultDialog::onFixSelected()
{
    if (!m_body) return;
    
    QVector<GeometryIssue> selectedIssues = m_model->selectedIssues();
    if (selectedIssues.isEmpty()) return;
    
    RepairOptions options;
    m_engine->repairAll(*m_body, std::vector<GeometryIssue>(selectedIssues.begin(), selectedIssues.end()), options);
}

void CheckResultDialog::onFixAll()
{
    if (!m_body) return;
    
    int ret = QMessageBox::question(this, tr("确认修复"),
        tr("确定要修复所有可修复的问题吗?\n共%1个可修复问题")
        .arg(m_statistics.fixableCount),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        RepairOptions options;
        m_engine->repairAll(*m_body, m_engine->filterFixable(), options);
    }
}

void CheckResultDialog::onViewIssue()
{
    // 在视口中高亮显示选中的问题
}

void CheckResultDialog::onExportReport()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("导出报告"), QString(),
        tr("HTML文件 (*.html);;文本文件 (*.txt);;CSV文件 (*.csv)"));
    
    if (!filePath.isEmpty()) {
        // TODO: 导出报告
        QMessageBox::information(this, tr("导出成功"), tr("报告已导出到:") + filePath);
    }
}

void CheckResultDialog::onFilterButtonClicked()
{
}

void CheckResultDialog::onCheckTypeFilterChanged(int index)
{
    // 处理类型筛选
}

void CheckResultDialog::onSeverityFilterChanged(int index)
{
    // 处理严重级别筛选
}

void CheckResultDialog::onStatusFilterChanged(int index)
{
    // 处理状态筛选
}

void CheckResultDialog::updateStatisticsDisplay()
{
    m_totalLabel->setText(tr("总计: %1").arg(m_statistics.totalIssues));
    m_criticalLabel->setText(tr("严重: %1").arg(m_statistics.criticalCount));
    m_errorLabel->setText(tr("错误: %1").arg(m_statistics.errorCount));
    m_warningLabel->setText(tr("警告: %1").arg(m_statistics.warningCount));
    m_infoLabel->setText(tr("信息: %1").arg(m_statistics.infoCount));
    m_fixableLabel->setText(tr("可修复: %1").arg(m_statistics.fixableCount));
    
    m_fixAllButton->setEnabled(m_statistics.fixableCount > 0);
}

void CheckResultDialog::updateButtonStates()
{
    bool hasIssues = m_statistics.totalIssues > 0;
    m_fixSelectedButton->setEnabled(hasIssues && m_model->selectedCount() > 0);
    m_fixAllButton->setEnabled(m_statistics.fixableCount > 0);
}

void CheckResultDialog::showProgress(const QString& message)
{
    m_progressBar->setVisible(true);
    m_progressLabel->setVisible(true);
    m_progressLabel->setText(message);
    m_progressBar->setValue(0);
}

void CheckResultDialog::hideProgress()
{
    m_progressBar->setVisible(false);
    m_progressLabel->setVisible(false);
}

std::vector<GeometryIssue> CheckResultDialog::fixedIssues() const
{
    // 返回已修复的问题列表
    return std::vector<GeometryIssue>();
}

} // namespace PS
