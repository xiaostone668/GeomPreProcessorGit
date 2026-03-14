#include "ui/PatchingDialog.h"
#include "geometry/PsBody.h"
#include <QComboBox>
#include <QSpinBox>
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

PatchingDialog::PatchingDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    
    setWindowTitle(tr("N边曲面补面"));
    resize(600, 500);
}

PatchingDialog::~PatchingDialog() = default;

void PatchingDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 方法选择
    QGridLayout* optionsLayout = new QGridLayout();
    
    optionsLayout->addWidget(new QLabel(tr("方法:")), 0, 0);
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem(tr("自动"), static_cast<int>(PatchingMethod::Auto));
    m_methodCombo->addItem(tr("曲面拟合"), static_cast<int>(PatchingMethod::GeomPlate));
    m_methodCombo->addItem(tr("Coons曲面"), static_cast<int>(PatchingMethod::Coons));
    m_methodCombo->addItem(tr("B-Spline"), static_cast<int>(PatchingMethod::BSpline));
    m_methodCombo->addItem(tr("直纹面"), static_cast<int>(PatchingMethod::Ruled));
    m_methodCombo->addItem(tr("填充曲面"), static_cast<int>(PatchingMethod::Filling));
    optionsLayout->addWidget(m_methodCombo, 0, 1);
    
    optionsLayout->addWidget(new QLabel(tr("次数:")), 1, 0);
    m_degreeSpin = new QSpinBox(this);
    m_degreeSpin->setRange(1, 10);
    m_degreeSpin->setValue(3);
    optionsLayout->addWidget(m_degreeSpin, 1, 1);
    
    optionsLayout->addWidget(new QLabel(tr("U向分段:")), 2, 0);
    m_uPatchesSpin = new QSpinBox(this);
    m_uPatchesSpin->setRange(2, 100);
    m_uPatchesSpin->setValue(10);
    optionsLayout->addWidget(m_uPatchesSpin, 2, 1);
    
    optionsLayout->addWidget(new QLabel(tr("V向分段:")), 3, 0);
    m_vPatchesSpin = new QSpinBox(this);
    m_vPatchesSpin->setRange(2, 100);
    m_vPatchesSpin->setValue(10);
    optionsLayout->addWidget(m_vPatchesSpin, 3, 1);
    
    optionsLayout->addWidget(new QLabel(tr("公差:")), 4, 0);
    m_toleranceSpin = new QDoubleSpinBox(this);
    m_toleranceSpin->setRange(1e-10, 1.0);
    m_toleranceSpin->setValue(1e-6);
    m_toleranceSpin->setDecimals(10);
    optionsLayout->addWidget(m_toleranceSpin, 4, 1);
    
    optionsLayout->addWidget(new QLabel(tr("最大误差:")), 5, 0);
    m_maxErrorSpin = new QDoubleSpinBox(this);
    m_maxErrorSpin->setRange(1e-10, 1.0);
    m_maxErrorSpin->setValue(1e-3);
    m_maxErrorSpin->setDecimals(10);
    optionsLayout->addWidget(m_maxErrorSpin, 5, 1);
    
    m_tangencyCheck = new QCheckBox(tr("保持切向连续"), this);
    optionsLayout->addWidget(m_tangencyCheck, 6, 0, 1, 2);
    
    m_enforceBoundsCheck = new QCheckBox(tr("强制边界约束"), this);
    m_enforceBoundsCheck->setChecked(true);
    optionsLayout->addWidget(m_enforceBoundsCheck, 7, 0, 1, 2);
    
    mainLayout->addLayout(optionsLayout);
    
    // 边界列表
    mainLayout->addWidget(new QLabel(tr("选中的边界:")));
    m_boundaryList = new QListWidget(this);
    mainLayout->addWidget(m_boundaryList);
    
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
    
    m_selectBoundaryButton = new QPushButton(tr("选择边界"), this);
    m_clearButton = new QPushButton(tr("清除"), this);
    m_autoPatchButton = new QPushButton(tr("自动补面"), this);
    m_patchButton = new QPushButton(tr("补面"), this);
    m_patchButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("取消"), this);
    
    buttonLayout->addWidget(m_selectBoundaryButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addWidget(m_autoPatchButton);
    buttonLayout->addWidget(m_patchButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void PatchingDialog::setupConnections()
{
    connect(m_patchButton, &QPushButton::clicked, this, &PatchingDialog::onPatch);
    connect(m_autoPatchButton, &QPushButton::clicked, this, &PatchingDialog::onAutoPatch);
    connect(m_selectBoundaryButton, &QPushButton::clicked, this, &PatchingDialog::onSelectBoundary);
    connect(m_clearButton, &QPushButton::clicked, this, &PatchingDialog::onClearSelection);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void PatchingDialog::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
}

void PatchingDialog::setSelectedEdges(const std::vector<Edge>& edges)
{
    m_selectedEdges = edges;
    updateBoundaryList();
}

PatchingOptions PatchingDialog::options() const
{
    PatchingOptions opts;
    opts.method = static_cast<PatchingMethod>(m_methodCombo->currentData().toInt());
    opts.degree = m_degreeSpin->value();
    opts.numUPatches = m_uPatchesSpin->value();
    opts.numVPatches = m_vPatchesSpin->value();
    opts.tolerance = m_toleranceSpin->value();
    opts.maxError = m_maxErrorSpin->value();
    opts.tangency = m_tangencyCheck->isChecked();
    opts.enforceBounds = m_enforceBoundsCheck->isChecked();
    return opts;
}

void PatchingDialog::onPatch()
{
    if (m_selectedEdges.size() < 3) {
        return;
    }
    
    showProgress(tr("执行补面..."));
    
    // 执行补面
    
    hideProgress();
    accept();
}

void PatchingDialog::onAutoPatch()
{
    if (!m_body) return;
    
    showProgress(tr("自动补面..."));
    
    // 自动查找所有需要补面的区域
    
    hideProgress();
    accept();
}

void PatchingDialog::onSelectBoundary()
{
    // 从视口中选择边界边
}

void PatchingDialog::onClearSelection()
{
    m_selectedEdges.clear();
    updateBoundaryList();
}

void PatchingDialog::updateBoundaryList()
{
    m_boundaryList->clear();
    for (size_t i = 0; i < m_selectedEdges.size(); i++) {
        m_boundaryList->addItem(tr("边 %1").arg(i + 1));
    }
}

void PatchingDialog::showProgress(const QString& message)
{
    m_isProcessing = true;
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
    m_statusLabel->setText(message);
}

void PatchingDialog::hideProgress()
{
    m_isProcessing = false;
    m_progressBar->setVisible(false);
    m_statusLabel->setVisible(false);
}

} // namespace PS
