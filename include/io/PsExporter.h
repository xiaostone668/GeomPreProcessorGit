#ifndef PSEXPORTER_H
#define PSEXPORTER_H

#include "geometry/PsBody.h"
#include <QString>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 导出结果
 */
struct ExportResult {
    bool success = false;
    QString errorMessage;
    int exportedEntityCount = 0;
};

/**
 * @brief 导出选项
 */
struct ExportOptions {
    bool includeAttributes = true;
    bool includeMaterials = false;
    double tolerance = 1e-6;
    QString version;
};

/**
 * @brief 导出器基类
 */
class Exporter {
public:
    virtual ~Exporter() = default;
    
    virtual ExportResult exportBody(const Body& body, const QString& filePath, 
                                    const ExportOptions& options = ExportOptions()) = 0;
    
    virtual ExportResult exportBodies(const std::vector<std::shared_ptr<Body>>& bodies, 
                                      const QString& filePath,
                                      const ExportOptions& options = ExportOptions()) = 0;
    
    virtual QStringList supportedFormats() const = 0;
    virtual QString formatDescription(const QString& format) const = 0;
    
    void setProgressCallback(std::function<void(int, const QString&)> callback) {
        m_progressCallback = callback;
    }
    
protected:
    void reportProgress(int percent, const QString& message) {
        if (m_progressCallback) {
            m_progressCallback(percent, message);
        }
    }
    
    std::function<void(int, const QString&)> m_progressCallback;
};

} // namespace PS

#endif // PSEXPORTER_H
