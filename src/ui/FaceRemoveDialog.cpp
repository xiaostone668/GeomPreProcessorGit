#include "ui/FaceRemoveDialog.h"
#include "geometry/PsBody.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

namespace PS {

FaceRemoveDialog::FaceRemoveDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    
    setWindowTitle(tr("删除面"));
    resize(600, 500);
}

FaceRemoveDialog::~FaceRemoveDialog() = default;

void FaceRemoveDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 选项
    QGridLayout* optionsLayout = new QGridLayout();
    
    optionsLayout->addWidget(new QLabel(tr("填补策略:")), 0, 0);
    m_strategyCombo = new QComboBox(this);
    m_strategyCombo->addItem(tr("自动"), static_cast<int>(HoleFillingStrategy::Auto));
    m_strategyCombo->addItem(tr("平面填充"), static_cast<int>(HoleFillingStrategy::Planar));
    m_strategyCombo->addItem(tr("最佳拟合"), static_cast<int>(HoleFillingStrategy::BestFit));
    m_strategyCombo->addItem(tr("切向连续"), static_cast<int>(HoleFillingStrategy::Tangent));
    m_strategyCombo->addItem(tr("补面引擎"), static_cast<int>(HoleFillingStrategy::Patch));
    m_strategyCombo->addItem(tr("保持开放"), static_cast<int>(HoleFillingStrategy::LeaveOpen));
    optionsLayout->addWidget(m_strategyCombo, 0, 1);
    
    optionsLayout->addWidget(new QLabel(tr("最大孔洞尺寸:")), 1, 0);
    m_maxHoleSizeSpin = new QDoubleSpinBox(this);
    m_maxHoleSizeSpin->setRange(0, 1e10);
    m_maxHoleSizeSpin->setValue(0);
    m_maxHoleSizeSpin->setSpecialValueText(tr("无限制"));
    optionsLayout->addWidget(m_maxHoleSizeSpin, 1, 1);
    
    optionsLayout->addWidget(new QLabel(tr("最大长宽比:")), 2, 0);
    m_maxAspectRatioSpin = new QDoubleSpinBox(this);
    m_maxAspectRatioSpin->setRange(1, 100);
    m_maxAspectRatioSpin->setValue(10);
    optionsLayout->addWidget(m_maxAspectRatioSpin, 2, 1);
    
    m_fillHolesCheck = new QCheckBox(tr("自动填补孔洞"), this);
    m_fillHolesCheck->setChecked(true);
    optionsLayout->addWidget(m_fillHolesCheck, 3, 0, 1, 2);
    
    m_preserveTopologyCheck = new QCheckBox(tr("保持拓扑结构"), this);
    m_preserveTopologyCheck->setChecked(true);
    optionsLayout->addWidget(m_preserveTopologyCheck, 4, 0, 1, 2);
    
    m_simplifyBoundaryCheck = new QCheckBox(tr("简化边界"), this);
    m_simplifyBoundaryCheck->setChecked(true);
    optionsLayout->addWidget(m_simplifyBoundaryCheck, 5, 0, 1, 2);
    
    mainLayout->addLayout(optionsLayout);
    
    // 面列表
    mainLayout->addWidget(new QLabel(tr("选中的面:")));
    m_faceList = new QListWidget(this);
    mainLayout->addWidget(m_faceList);
    
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
    
    m_selectButton = new QPushButton(tr("选择面"), this);
    m_clearButton = new QPushButton(tr("清除"), this);
    m_analyzeButton = new QPushButton(tr("分析"), this);
    m_smartRemoveButton = new QPushButton(tr("智能删除"), this);
    m_removeButton = new QPushButton(tr("删除"), this);
    m_removeButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("取消"), this);
    
    buttonLayout->addWidget(m_selectButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addWidget(m_analyzeButton);
    buttonLayout->addWidget(m_smartRemoveButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void FaceRemoveDialog::setupConnections()
{
    connect(m_removeButton, &QPushButton::clicked, this, &FaceRemoveDialog::onRemove);
    connect(m_smartRemoveButton, &QPushButton::clicked, this, &FaceRemoveDialog::onSmartRemove);
    connect(m_analyzeButton, &QPushButton::clicked, this, &FaceRemoveDialog::onAnalyzeHoles);
    connect(m_selectButton, &QPushButton::clicked, this, &FaceRemoveDialog::onSelectFaces);
    connect(m_clearButton, &QPushButton::clicked, this, &FaceRemoveDialog::onClearSelection);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void FaceRemoveDialog::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
}

void FaceRemoveDialog::setSelectedFaces(const std::vector<Face>& faces)
{
    m_selectedFaces = faces;
    updateFaceList();
}

FaceRemoveOptions FaceRemoveDialog::options() const
{
    FaceRemoveOptions opts;
    opts.fillingStrategy = static_cast<HoleFillingStrategy>(m_strategyCombo->currentData().toInt());
    opts.maxHoleSize = m_maxHoleSizeSpin->value();
    opts.maxHoleAspectRatio = m_maxAspectRatioSpin->value();
    opts.fillHoles = m_fillHolesCheck->isChecked();
    opts.preserveTopology = m_preserveTopologyCheck->isChecked();
    opts.simplifyBoundary = m_simplifyBoundaryCheck->isChecked();
    return opts;
}

void FaceRemoveDialog::onRemove()
{
    if (m_selectedFaces.empty()) return;
    
    showProgress(tr("执行删除..."));
    
    // 执行删除
    
    hideProgress();
    accept();
}

void FaceRemoveDialog::onSmartRemove()
{
    if (m_selectedFaces.empty()) return;
    
    showProgress(tr("智能删除..."));
    
    // 智能删除逻辑
    
    hideProgress();
    accept();
}

void FaceRemoveDialog::onAnalyzeHoles()
{
    if (!m_body) return;
    
    showProgress(tr("分析孔洞..."));
    
    // 分析删除后的孔洞
    
    hideProgress();
}

void FaceRemoveDialog::onSelectFaces()
{
    // 从视口中选择面
}

void FaceRemoveDialog::onClearSelection()
{
    m_selectedFaces.clear();
    updateFaceList();
}

void FaceRemoveDialog::onPreview()
{
    // 预览删除结果
}

void FaceRemoveDialog::updateFaceList()
{
    m_faceList->clear();
    for (size_t i = 0; i < m_selectedFaces.size(); i++) {
        m_faceList->addItem(tr("面 %1").arg(i + 1));
    }
}

void FaceRemoveDialog::showProgress(const QString& message)
{
    m_isProcessing = true;
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
    m_statusLabel->setText(message);
}

void FaceRemoveDialog::hideProgress()
{
    m_isProcessing = false;
    m_progressBar->setVisible(false);
    m_statusLabel->setVisible(false);
}

} // namespace PS
