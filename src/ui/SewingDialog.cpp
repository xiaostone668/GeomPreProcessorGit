#include "ui/SewingDialog.h"
#include "geometry/PsBody.h"
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

namespace PS {

SewingDialog::SewingDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    
    setWindowTitle(tr("缝合"));
    resize(500, 400);
}

SewingDialog::~SewingDialog() = default;

void SewingDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QGridLayout* optionsLayout = new QGridLayout();
    
    optionsLayout->addWidget(new QLabel(tr("公差:")), 0, 0);
    m_toleranceSpin = new QDoubleSpinBox(this);
    m_toleranceSpin->setRange(1e-10, 1.0);
    m_toleranceSpin->setValue(1e-6);
    m_toleranceSpin->setDecimals(10);
    optionsLayout->addWidget(m_toleranceSpin, 0, 1);
    
    optionsLayout->addWidget(new QLabel(tr("最大公差:")), 1, 0);
    m_maxToleranceSpin = new QDoubleSpinBox(this);
    m_maxToleranceSpin->setRange(1e-10, 10.0);
    m_maxToleranceSpin->setValue(1e-3);
    m_maxToleranceSpin->setDecimals(10);
    optionsLayout->addWidget(m_maxToleranceSpin, 1, 1);
    
    m_nonManifoldCheck = new QCheckBox(tr("允许非流形"), this);
    optionsLayout->addWidget(m_nonManifoldCheck, 2, 0, 1, 2);
    
    m_createShellsCheck = new QCheckBox(tr("创建壳体"), this);
    m_createShellsCheck->setChecked(true);
    optionsLayout->addWidget(m_createShellsCheck, 3, 0, 1, 2);
    
    m_createSolidsCheck = new QCheckBox(tr("创建实体"), this);
    m_createSolidsCheck->setChecked(true);
    optionsLayout->addWidget(m_createSolidsCheck, 4, 0, 1, 2);
    
    mainLayout->addLayout(optionsLayout);
    
    // 预览
    m_previewLabel = new QLabel(tr("预览: 尚未执行"), this);
    mainLayout->addWidget(m_previewLabel);
    
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
    
    m_analyzeButton = new QPushButton(tr("分析"), this);
    m_previewButton = new QPushButton(tr("预览"), this);
    m_smartSewButton = new QPushButton(tr("智能缝合"), this);
    m_sewButton = new QPushButton(tr("缝合"), this);
    m_sewButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("取消"), this);
    
    buttonLayout->addWidget(m_analyzeButton);
    buttonLayout->addWidget(m_previewButton);
    buttonLayout->addWidget(m_smartSewButton);
    buttonLayout->addWidget(m_sewButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void SewingDialog::setupConnections()
{
    connect(m_sewButton, &QPushButton::clicked, this, &SewingDialog::onSew);
    connect(m_smartSewButton, &QPushButton::clicked, this, &SewingDialog::onSmartSew);
    connect(m_analyzeButton, &QPushButton::clicked, this, &SewingDialog::onAnalyze);
    connect(m_previewButton, &QPushButton::clicked, this, &SewingDialog::onPreview);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SewingDialog::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
}

SewingOptions SewingDialog::options() const
{
    SewingOptions opts;
    opts.tolerance = m_toleranceSpin->value();
    opts.maxTolerance = m_maxToleranceSpin->value();
    opts.allowNonManifold = m_nonManifoldCheck->isChecked();
    opts.createShells = m_createShellsCheck->isChecked();
    opts.createSolids = m_createSolidsCheck->isChecked();
    return opts;
}

void SewingDialog::onSew()
{
    if (!m_body) return;
    
    showProgress(tr("执行缝合..."));
    
    // 执行缝合
    m_resultBody = m_body->sewn(options());
    
    hideProgress();
    accept();
}

void SewingDialog::onSmartSew()
{
    if (!m_body) return;
    
    showProgress(tr("智能缝合..."));
    
    // 智能缝合逻辑
    
    hideProgress();
    accept();
}

void SewingDialog::onAnalyze()
{
    if (!m_body) return;
    
    showProgress(tr("分析中..."));
    
    // 分析自由边
    int freeEdgeCount = 0;
    for (const auto& edge : m_body->edges()) {
        auto faces = edge.faces();
        if (faces.size() == 1) {
            freeEdgeCount++;
        }
    }
    
    m_previewLabel->setText(tr("分析结果: %1条自由边").arg(freeEdgeCount));
    
    hideProgress();
}

void SewingDialog::onPreview()
{
    // 预览缝合结果
}

void SewingDialog::showProgress(const QString& message)
{
    m_isProcessing = true;
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
    m_statusLabel->setText(message);
}

void SewingDialog::hideProgress()
{
    m_isProcessing = false;
    m_progressBar->setVisible(false);
    m_statusLabel->setVisible(false);
}

} // namespace PS
