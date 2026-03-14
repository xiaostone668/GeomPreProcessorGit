#include <QtTest>
#include "core/BeamExtractEngine.h"
#include "geometry/PsBody.h"

using namespace PS;

class TestBeamExtract : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testBeamExtractOptions();
    void testProfileRecognition();
};

void TestBeamExtract::initTestCase()
{
}

void TestBeamExtract::testBeamExtractOptions()
{
    BeamExtractOptions options;
    QVERIFY(options.useSkeleton);
    QVERIFY(options.detectBranches);
}

void TestBeamExtract::testProfileRecognition()
{
    // 测试截面识别
}

QTEST_MAIN(TestBeamExtract)
#include "test_beam_extract.moc"
