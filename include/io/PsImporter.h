#ifndef PSIMPORTER_H
#define PSIMPORTER_H

#include "geometry/PsBody.h"
#include <QString>
#include <vector>
#include <memory>

namespace PS {

/**
 * @brief 导入结果
 */
struct ImportResult {
    bool success = false;
    std::vector<std::shared_ptr<Body>> bodies;
    QString errorMessage;
    QString warningMessage;
    int entityCount = 0;
};

/**
 * @brief 导入选项
 */
struct ImportOptions {
    bool mergeSolids = false;
    bool healGeometry = true;
    double tolerance = 1e-6;
    bool importAttributes = true;
    bool importMaterials = false;
};

/**
 * @brief 导入器基类
 */
class Importer {
public:
    virtual ~Importer() = default;
    
    virtual ImportResult import(const QString& filePath, const ImportOptions& options = ImportOptions()) = 0;
    
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

#endif // PSIMPORTER_H
