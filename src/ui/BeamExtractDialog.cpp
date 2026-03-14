#include "ui/BeamExtractDialog.h"
#include "geometry/PsBody.h"
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

namespace PS {

BeamExtractDialog::BeamExtractDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    
    setWindowTitle(tr("抽梁"));
    resize(800, 600);
}

BeamExtractDialog::~BeamExtractDialog() = default;

void BeamExtractDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 选项
    QGridLayout* optionsLayout = new QGridLayout();
    
    optionsLayout->addWidget(new QLabel(tr("最小长度:")), 0, 0);
    m_minLengthSpin = new QDoubleSpinBox(this);
    m_minLengthSpin->setRange(0, 1e6);
    m_minLengthSpin->setValue(0.001);
    m_minLengthSpin->setDecimals(6);
    optionsLayout->addWidget(m_minLengthSpin, 0, 1);
    
    optionsLayout->addWidget(new QLabel(tr("最小截面面积:")), 0, 2);
    m_minAreaSpin = new QDoubleSpinBox(this);
    m_minAreaSpin->setRange(0, 1e6);
    m_minAreaSpin->setValue(1e-6);
    m_minAreaSpin->setDecimals(10);
    optionsLayout->addWidget(m_minAreaSpin, 0, 3);
    
    optionsLayout->addWidget(new QLabel(tr("最大曲率:")), 1, 0);
    m_maxCurvatureSpin = new QDoubleSpinBox(this);
    m_maxCurvatureSpin->setRange(0, 100);
    m_maxCurvatureSpin->setValue(1.0);
    optionsLayout->addWidget(m_maxCurvatureSpin, 1, 1);
    
    m_useSkeletonCheck = new QCheckBox(tr("使用骨架提取"), this);
    m_useSkeletonCheck->setChecked(true);
    optionsLayout->addWidget(m_useSkeletonCheck, 2, 0, 1, 2);
    
    m_useFeatureCheck = new QCheckBox(tr("使用特征识别"), this);
    m_useFeatureCheck->setChecked(true);
    optionsLayout->addWidget(m_useFeatureCheck, 2, 2, 1, 2);
    
    m_detectBranchesCheck = new QCheckBox(tr("检测分支"), this);
    m_detectBranchesCheck->setChecked(true);
    optionsLayout->addWidget(m_detectBranchesCheck, 3, 0, 1, 2);
    
    m_detectJunctionsCheck = new QCheckBox(tr("检测连接点"), this);
    m_detectJunctionsCheck->setChecked(true);
    optionsLayout->addWidget(m_detectJunctionsCheck, 3, 2, 1, 2);
    
    m_allowTaperedCheck = new QCheckBox(tr("允许变截面"), this);
    m_allowTaperedCheck->setChecked(true);
    optionsLayout->addWidget(m_allowTaperedCheck, 4, 0, 1, 2);
    
    mainLayout->addLayout(optionsLayout);
    
    // 结果表格
    m_summaryLabel = new QLabel(tr("尚未提取"), this);
    mainLayout->addWidget(m_summaryLabel);
    
    m_beamTable = new QTableWidget(this);
    m_beamTable->setColumnCount(6);
    m_beamTable->setHorizontalHeaderLabels({
        tr("ID"), tr("长度"), tr("段数"), tr("截面类型"), tr("变截面"), tr("体积")
    });
    mainLayout->addWidget(m_beamTable, 1);
    
    // 进度
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setVisible(false);
    mainLayout->addWidget(m_statusLabel);
    
    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_extractButton = new QPushButton(tr("提取梁"), this);
    m_extractSkeletonButton = new QPushButton(tr("提取骨架"), this);
    m_exportButton = new QPushButton(tr("导出"), this);
    m_previewButton = new QPushButton(tr("预览"), this);
    m_cancelButton = new QPushButton(tr("关闭"), this);
    
    buttonLayout->addWidget(m_extractButton);
    buttonLayout->addWidget(m_extractSkeletonButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_previewButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void BeamExtractDialog::setupConnections()
{
    connect(m_extractButton, &QPushButton::clicked, this, &BeamExtractDialog::onExtract);
    connect(m_extractSkeletonButton, &QPushButton::clicked, this, &BeamExtractDialog::onExtractSkeleton);
    connect(m_exportButton, &QPushButton::clicked, this, &BeamExtractDialog::onExport);
    connect(m_previewButton, &QPushButton::clicked, this, &BeamExtractDialog::onPreview);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::accept);
}

void BeamExtractDialog::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
}

BeamExtractOptions BeamExtractDialog::options() const
{
    BeamExtractOptions opts;
    opts.minBeamLength = m_minLengthSpin->value();
    opts.minCrossSectionArea = m_minAreaSpin->value();
    opts.maxCurvature = m_maxCurvatureSpin->value();
    opts.useSkeleton = m_useSkeletonCheck->isChecked();
    opts.useFeatureRecognition = m_useFeatureCheck->isChecked();
    opts.detectBranches = m_detectBranchesCheck->isChecked();
    opts.detectJunctions = m_detectJunctionsCheck->isChecked();
    opts.allowTapered = m_allowTaperedCheck->isChecked();
    return opts;
}

void BeamExtractDialog::onExtract()
{
    if (!m_body) return;
    
    showProgress(tr("提取梁结构..."));
    
    // 执行提取
    
    hideProgress();
    updateResults();
}

void BeamExtractDialog::onExtractSkeleton()
{
    if (!m_body) return;
    
    showProgress(tr("提取骨架..."));
    
    // 仅提取骨架
    
    hideProgress();
}

void BeamExtractDialog::onExport()
{
    // 导出梁信息
}

void BeamExtractDialog::onPreview()
{
    // 预览提取结果
}

void BeamExtractDialog::onViewBeam()
{
    // 查看单个梁
}

void BeamExtractDialog::onViewSection()
{
    // 查看截面
}

void BeamExtractDialog::updateResults()
{
    // 更新结果显示
    m_summaryLabel->setText(tr("提取到%1个梁结构").arg(m_beams.size()));
}

void BeamExtractDialog::showProgress(const QString& message)
{
    m_isProcessing = true;
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
    m_statusLabel->setText(message);
}

void BeamExtractDialog::hideProgress()
{
    m_isProcessing = false;
    m_progressBar->setVisible(false);
    m_statusLabel->setVisible(false);
}

} // namespace PS
