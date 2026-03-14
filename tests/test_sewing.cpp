#include <QtTest>
#include "core/SewingEngine.h"
#include "geometry/PsBody.h"

using namespace PS;

class TestSewing : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSewingOptions();
    void testSewBody();
};

void TestSewing::initTestCase()
{
    // 初始化测试环境
}

void TestSewing::cleanupTestCase()
{
    // 清理测试环境
}

void TestSewing::testSewingOptions()
{
    SewingOptions options;
    QCOMPARE(options.tolerance, 1e-6);
    QCOMPARE(options.createSolids, true);
}

void TestSewing::testSewBody()
{
    SewingEngine engine;
    
    // 创建测试体
    Body body = Body::createSolid();
    
    // 执行缝合
    SewingOptions options;
    options.tolerance = 1e-5;
    
    // Body result = engine.sew(body, options);
    // QVERIFY(result.isValid());
}

QTEST_MAIN(TestSewing)
#include "test_sewing.moc"
