#ifndef PSKERNEL_H
#define PSKERNEL_H

#include "geometry/PsTypes.h"
#include <QString>
#include <QException>
#include <memory>

namespace PS {

// Parasolid异常
class PsException : public QException {
public:
    explicit PsException(const QString& message);
    explicit PsException(const char* message);
    
    QString message() const { return m_message; }
    const char* what() const noexcept override;
    
    void raise() const override { throw *this; }
    QException* clone() const override { return new PsException(*this); }
    
private:
    QString m_message;
    mutable std::string m_whatBuffer;
};

// Parasolid错误码
enum class ErrorCode {
    NoError = 0,
    NullEntity = 1,
    InvalidEntity = 2,
    OutOfMemory = 3,
    DivisionByZero = 4,
    SingularTransformation = 5,
    BadParameter = 6,
    UnknownError = 99
};

// 内核初始化选项
struct KernelOptions {
    bool enableUndo = true;
    size_t undoStackSize = 100;
    bool enableLogging = false;
    QString logFilePath;
    double defaultTolerance = 1e-6;
};

/**
 * @brief Parasolid内核管理类
 * 
 * 负责Parasolid内核的初始化和终止
 * 提供错误处理和实体管理功能
 */
class Kernel {
public:
    // 获取单例实例
    static Kernel& instance();
    
    // 禁止拷贝
    Kernel(const Kernel&) = delete;
    Kernel& operator=(const Kernel&) = delete;
    
    // 初始化/终止
    bool initialize(const KernelOptions& options = KernelOptions());
    bool terminate();
    bool isInitialized() const { return m_initialized; }
    
    // 错误处理
    ErrorCode getLastError() const { return m_lastError; }
    QString getLastErrorString() const;
    void clearError();
    
    // 实体有效性检查
    bool isValidEntity(PK_ENTITY_t entity) const;
    bool isNullEntity(PK_ENTITY_t entity) const { return entity == PK_ENTITY_null; }
    EntityType getEntityType(PK_ENTITY_t entity) const;
    
    // 实体管理
    void deleteEntity(PK_ENTITY_t entity);
    void deleteEntities(const std::vector<PK_ENTITY_t>& entities);
    
    // Undo/Redo
    bool canUndo() const;
    bool canRedo() const;
    bool undo();
    bool redo();
    void clearUndoStack();
    
    // 选项
    const KernelOptions& options() const { return m_options; }
    void setOptions(const KernelOptions& options);
    
    // 版本信息
    static QString versionString();
    static int versionMajor();
    static int versionMinor();
    static int versionPatch();

private:
    Kernel();
    ~Kernel();
    
    void setError(ErrorCode code);
    
    bool m_initialized = false;
    ErrorCode m_lastError = ErrorCode::NoError;
    KernelOptions m_options;
};

// 内核守卫 - RAII方式管理内核生命周期
class KernelGuard {
public:
    explicit KernelGuard(const KernelOptions& options = KernelOptions());
    ~KernelGuard();
    
    bool isInitialized() const { return m_initialized; }
    
private:
    bool m_initialized;
};

// 实体守卫 - RAII方式管理实体生命周期
class EntityGuard {
public:
    explicit EntityGuard(PK_ENTITY_t entity);
    ~EntityGuard();
    
    // 释放管理权 (不删除实体)
    void release();
    
    // 重置为新的实体
    void reset(PK_ENTITY_t entity);
    
    PK_ENTITY_t entity() const { return m_entity; }
    
private:
    PK_ENTITY_t m_entity;
    bool m_owned;
};

// 错误处理宏
#define PS_THROW_IF_ERROR(cond, msg) \
    do { if (cond) throw PsException(msg); } while(0)

#define PS_CHECK_ENTITY(entity) \
    do { if (!Kernel::instance().isValidEntity(entity)) \
        throw PsException(QString("Invalid entity: %1").arg(entity)); } while(0)

#define PS_CHECK_NULL(entity) \
    do { if (Kernel::instance().isNullEntity(entity)) \
        throw PsException("Null entity"); } while(0)

} // namespace PS

#endif // PSKERNEL_H
