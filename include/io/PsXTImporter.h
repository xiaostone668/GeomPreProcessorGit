#ifndef PSXTIMPORTER_H
#define PSXTIMPORTER_H

#include "PsImporter.h"

namespace PS {

/**
 * @brief Parasolid XT文件导入器
 */
class XTImporter : public Importer {
public:
    XTImporter();
    ~XTImporter() override;
    
    ImportResult import(const QString& filePath, const ImportOptions& options = ImportOptions()) override;
    
    QStringList supportedFormats() const override;
    QString formatDescription(const QString& format) const override;
    
    // XT特有功能
    ImportResult importFromMemory(const QByteArray& data, const ImportOptions& options = ImportOptions());
};

} // namespace PS

#endif // PSXTIMPORTER_H
