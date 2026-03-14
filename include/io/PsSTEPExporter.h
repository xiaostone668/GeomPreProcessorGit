#ifndef PSSTEPEXPORTER_H
#define PSSTEPEXPORTER_H

#include "PsExporter.h"

namespace PS {

/**
 * @brief STEP文件导出器
 */
class STEPExporter : public Exporter {
public:
    STEPExporter();
    ~STEPExporter() override;
    
    ExportResult exportBody(const Body& body, const QString& filePath, 
                           const ExportOptions& options = ExportOptions()) override;
    
    ExportResult exportBodies(const std::vector<std::shared_ptr<Body>>& bodies, 
                             const QString& filePath,
                             const ExportOptions& options = ExportOptions()) override;
    
    QStringList supportedFormats() const override;
    QString formatDescription(const QString& format) const override;
};

} // namespace PS

#endif // PSSTEPEXPORTER_H
