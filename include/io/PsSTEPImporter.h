#ifndef PSSTEPIMPORTER_H
#define PSSTEPIMPORTER_H

#include "PsImporter.h"

namespace PS {

/**
 * @brief STEP文件导入器
 */
class STEPImporter : public Importer {
public:
    STEPImporter();
    ~STEPImporter() override;
    
    ImportResult import(const QString& filePath, const ImportOptions& options = ImportOptions()) override;
    
    QStringList supportedFormats() const override;
    QString formatDescription(const QString& format) const override;
};

} // namespace PS

#endif // PSSTEPIMPORTER_H
