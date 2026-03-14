#ifndef BEAMEXTRACTDIALOG_H
#define BEAMEXTRACTDIALOG_H

#include "geometry/PsTypes.h"
#include "core/BeamExtractEngine.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QLabel;
class QTableWidget;
class QTabWidget;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 抽梁对话框
 */
class BeamExtractDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit BeamExtractDialog(QWidget* parent = nullptr);
    ~BeamExtractDialog();
    
    void setBody(const std::shared_ptr<Body>& body);
    
    BeamExtractOptions options() const;
    
public slots:
    void onExtract();
    void onExtractSkeleton();
    void onExport();
    void onPreview();
    void onViewBeam();
    void onViewSection();
    
private:
    void setupUI();
    void setupConnections();
    void updateResults();
    void showProgress(const QString& message);
    void hideProgress();
    
    QDoubleSpinBox* m_minLengthSpin = nullptr;
    QDoubleSpinBox* m_minAreaSpin = nullptr;
    QDoubleSpinBox* m_maxCurvatureSpin = nullptr;
    QCheckBox* m_useSkeletonCheck = nullptr;
    QCheckBox* m_useFeatureCheck = nullptr;
    QCheckBox* m_detectBranchesCheck = nullptr;
    QCheckBox* m_detectJunctionsCheck = nullptr;
    QCheckBox* m_allowTaperedCheck = nullptr;
    
    QTableWidget* m_beamTable = nullptr;
    
    QPushButton* m_extractButton = nullptr;
    QPushButton* m_extractSkeletonButton = nullptr;
    QPushButton* m_exportButton = nullptr;
    QPushButton* m_previewButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_summaryLabel = nullptr;
    
    std::shared_ptr<Body> m_body;
    std::vector<BeamInfo> m_beams;
    std::vector<Edge> m_skeletonEdges;
    BeamExtractResult m_result;
    
    bool m_isProcessing = false;
};

} // namespace PS

#endif // BEAMEXTRACTDIALOG_H
