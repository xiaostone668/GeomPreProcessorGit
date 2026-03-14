#ifndef SEWINGDIALOG_H
#define SEWINGDIALOG_H

#include "geometry/PsTypes.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QLabel;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 缝合对话框
 */
class SewingDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit SewingDialog(QWidget* parent = nullptr);
    ~SewingDialog();
    
    void setBody(const std::shared_ptr<Body>& body);
    
    SewingOptions options() const;
    
public slots:
    void onSew();
    void onSmartSew();
    void onAnalyze();
    void onPreview();
    
private:
    void setupUI();
    void setupConnections();
    void updatePreview();
    void showProgress(const QString& message);
    void hideProgress();
    
    QDoubleSpinBox* m_toleranceSpin = nullptr;
    QDoubleSpinBox* m_maxToleranceSpin = nullptr;
    QCheckBox* m_nonManifoldCheck = nullptr;
    QCheckBox* m_createShellsCheck = nullptr;
    QCheckBox* m_createSolidsCheck = nullptr;
    
    QPushButton* m_sewButton = nullptr;
    QPushButton* m_smartSewButton = nullptr;
    QPushButton* m_analyzeButton = nullptr;
    QPushButton* m_previewButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_previewLabel = nullptr;
    
    std::shared_ptr<Body> m_body;
    std::shared_ptr<Body> m_resultBody;
    
    bool m_isProcessing = false;
};

} // namespace PS

#endif // SEWINGDIALOG_H
