#include <QtTest>
#include "core/PatchingEngine.h"
#include "geometry/PsEdge.h"

using namespace PS;

class TestPatching : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testPatchingOptions();
    void testMethodRecommendation();
};

void TestPatching::initTestCase()
{
}

void TestPatching::testPatchingOptions()
{
    PatchingOptions options;
    QCOMPARE(options.degree, 3);
    QVERIFY(options.enforceBounds);
}

void TestPatching::testMethodRecommendation()
{
    NsidedBoundary boundary;
    boundary.numSides = 4;
    
    PatchingMethod method = PatchingEngine::recommendMethod(boundary);
    QCOMPARE(method, PatchingMethod::Coons);
}

QTEST_MAIN(TestPatching)
#include "test_patching.moc"
