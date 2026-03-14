#ifndef FACEREMOVEDIALOG_H
#define FACEREMOVEDIALOG_H

#include "geometry/PsTypes.h"
#include "core/FaceRemoveEngine.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QLabel;
class QListWidget;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 删除面对话框
 */
class FaceRemoveDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit FaceRemoveDialog(QWidget* parent = nullptr);
    ~FaceRemoveDialog();
    
    void setBody(const std::shared_ptr<Body>& body);
    void setSelectedFaces(const std::vector<Face>& faces);
    
    FaceRemoveOptions options() const;
    
public slots:
    void onRemove();
    void onSmartRemove();
    void onAnalyzeHoles();
    void onSelectFaces();
    void onClearSelection();
    void onPreview();
    
private:
    void setupUI();
    void setupConnections();
    void updateFaceList();
    void showProgress(const QString& message);
    void hideProgress();
    
    QComboBox* m_strategyCombo = nullptr;
    QDoubleSpinBox* m_maxHoleSizeSpin = nullptr;
    QDoubleSpinBox* m_maxAspectRatioSpin = nullptr;
    QCheckBox* m_fillHolesCheck = nullptr;
    QCheckBox* m_preserveTopologyCheck = nullptr;
    QCheckBox* m_simplifyBoundaryCheck = nullptr;
    
    QListWidget* m_faceList = nullptr;
    
    QPushButton* m_removeButton = nullptr;
    QPushButton* m_smartRemoveButton = nullptr;
    QPushButton* m_analyzeButton = nullptr;
    QPushButton* m_selectButton = nullptr;
    QPushButton* m_clearButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_previewLabel = nullptr;
    
    std::shared_ptr<Body> m_body;
    std::vector<Face> m_selectedFaces;
    std::shared_ptr<Body> m_resultBody;
    
    bool m_isProcessing = false;
};

} // namespace PS

#endif // FACEREMOVEDIALOG_H
