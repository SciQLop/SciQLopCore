#include <QObject>
#include <QtTest>
#include <QUuid>
#include <limits>


#include <Data/DateTimeRange.h>
#include <Data/DateTimeRangeHelper.h>
#include <Common/Numeric.h>

Q_DECLARE_METATYPE(Seconds<double>);
Q_DECLARE_METATYPE(DateTimeRangeTransformation);

DateTimeRange computeZoom(QDateTime start, QDateTime stop, double zoom)
{
    double start_epoch = start.toMSecsSinceEpoch();
    double stop_epoch = stop.toMSecsSinceEpoch();
    auto delta = stop_epoch - start_epoch;
    auto dt_ms = (zoom-1.)*(double(delta))/2.;
    return DateTimeRange{(start_epoch - dt_ms)/1000., (stop_epoch + dt_ms)/1000.};
}

class TestDateTimeRange: public QObject {
    Q_OBJECT

private slots:
    void testRangeDelta_data()
    {
        QTest::addColumn<QDateTime>("tstart");
        QTest::addColumn<QDateTime>("tend");
        QTest::addColumn<double>("expected");
        auto now = QDateTime::currentDateTime();
        auto yesterday = QDateTime::currentDateTime().addDays(-1);
        QTest::newRow("No delta") << now << now << 0.;
        QTest::newRow("One day delta") << yesterday << now << 60.*60.*24.;
        QTest::newRow("Minus one day delta") << now << yesterday << -60.*60.*24.;
    }

    void testRangeDelta()
    {
        QFETCH(QDateTime,tstart);
        QFETCH(QDateTime,tend);
        QFETCH(double,expected);
        auto range = DateTimeRange::fromDateTime(tstart, tend);
        double delta = range.delta();
        // Since it is built from QDateTime don't expect better resolution
        QVERIFY((delta-expected) <= 0.002);
    }

    void testRangeShift_data()
    {
        QTest::addColumn<DateTimeRange>("initial");
        QTest::addColumn<Seconds<double>>("shift");
        QTest::addColumn<DateTimeRange>("expected");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("No shift") << range << Seconds<double>{0.} << range;

        QTest::newRow("One milisecond left") << range << Seconds<double>{-.001} <<
                                            DateTimeRange::fromDateTime(yestd.addMSecs(-1.), now.addMSecs(-1.));
        QTest::newRow("One milisecond right") << range << Seconds<double>{.001} <<
                                             DateTimeRange::fromDateTime(yestd.addMSecs(1.), now.addMSecs(1.));
        QTest::newRow("One second left") << range << Seconds<double>{-1.} <<
                                            DateTimeRange::fromDateTime(yestd.addSecs(-1.), now.addSecs(-1.));
        QTest::newRow("One second right") << range << Seconds<double>{1.} <<
                                             DateTimeRange::fromDateTime(yestd.addSecs(1.), now.addSecs(1.));
        QTest::newRow("One year left") << range << Seconds<double>{-365.*24.*60.*60.} <<
                                             DateTimeRange::fromDateTime(yestd.addYears(-1.), now.addYears(-1.));
        QTest::newRow("One year right") << range << Seconds<double>{365.*24.*60.*60.} <<
                                             DateTimeRange::fromDateTime(yestd.addYears(1.), now.addYears(1.));
    }
    void testRangeShift()
    {
        QFETCH(DateTimeRange,initial);
        QFETCH(Seconds<double>,shift);
        QFETCH(DateTimeRange,expected);
        QCOMPARE(initial+shift, expected);
    }

    void testRangeZoom_data()
    {
        QTest::addColumn<DateTimeRange>("initial");
        QTest::addColumn<double>("zoom");
        QTest::addColumn<DateTimeRange>("expected");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("No zoom") << range << 1. << range;

        QTest::newRow("Zoom IN 0.001") << range << 1.001 <<
                                            computeZoom(yestd, now, 1.001);
        QTest::newRow("Zoom OUT 0.001") << range << 0.999 <<
                                            computeZoom(yestd, now, 0.999);
    }
    void testRangeZoom()
    {
        QFETCH(DateTimeRange,initial);
        QFETCH(double,zoom);
        QFETCH(DateTimeRange,expected);
        QCOMPARE(initial*zoom, expected);
    }

    void testRangeContains_data()
    {
        QTest::addColumn<DateTimeRange>("range");
        QTest::addColumn<DateTimeRange>("range2");
        QTest::addColumn<bool>("contains");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("Same range") << range << range << true;
        QTest::newRow("Smaller range") << range << range * 0.8 << true;
        QTest::newRow("Bigger range") << range << range * 1.2 << false;
        QTest::newRow("Shifted range with overlap") << range << range + Seconds<double>{1000.} << false;
        QTest::newRow("Shifted range without overlap") << range << range + Seconds<double>{24.*60.*60.*10} << false;
    }

    void testRangeContains()
    {
        QFETCH(DateTimeRange,range);
        QFETCH(DateTimeRange,range2);
        QFETCH(bool,contains);
        QCOMPARE(range.contains(range2), contains);
    }

    void testRangeIntersect_data()
    {
        QTest::addColumn<DateTimeRange>("range");
        QTest::addColumn<DateTimeRange>("range2");
        QTest::addColumn<bool>("contains");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto tomorrow = QDateTime::currentDateTime().addDays(1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        auto range2 = DateTimeRange::fromDateTime(now, tomorrow);
        QTest::newRow("Same range") << range << range << true;
        QTest::newRow("Smaller range") << range << range * 0.8 << true;
        QTest::newRow("Bigger range") << range << range * 1.2 << true;
        QTest::newRow("Shifted range with overlap") << range << range + Seconds<double>{1000.} << true;
        QTest::newRow("Shifted range with overlaping boundary") << range << range2 << true;
        QTest::newRow("Shifted range .1 seonds outside") << range << range2 + Seconds<double>{.1} << false;
        QTest::newRow("Shifted range without overlap") << range << range + Seconds<double>{24.*60.*60.*10} << false;
    }

    void testRangeIntersect()
    {
        QFETCH(DateTimeRange,range);
        QFETCH(DateTimeRange,range2);
        QFETCH(bool,contains);
        QCOMPARE(range.intersect(range2), contains);
    }

    void testRangeTransformations_data()
    {
        QTest::addColumn<DateTimeRange>("range1");
        QTest::addColumn<DateTimeRange>("range2");
        QTest::addColumn<DateTimeRangeTransformation>("transformation");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);

        QTest::newRow("Same range") << range << range << DateTimeRangeTransformation{1.,Seconds<double>{0.}};

        QTest::newRow("Transformatio 1.1x + 10s") << range << range*1.1 + Seconds<double>{10.}
                                                  << DateTimeRangeTransformation{1.1, Seconds<double>{10.}};

        QTest::newRow("Transformatio 1.x + 10s") << range << range*1. + Seconds<double>{10.}
                                                  << DateTimeRangeTransformation{1., Seconds<double>{10.}};

        QTest::newRow("Transformatio 1.1x + 0s") << range << range*1.1 + Seconds<double>{0.}
                                                  << DateTimeRangeTransformation{1.1,Seconds<double>{0.}};

        QTest::newRow("Transformatio 0.9x - 10s") << range << range*0.9 + Seconds<double>{-10.}
                                                  << DateTimeRangeTransformation{0.9, Seconds<double>{-10.}};

    }
    void testRangeTransformations()
    {
        QFETCH(DateTimeRange,range1);
        QFETCH(DateTimeRange,range2);
        QFETCH(DateTimeRangeTransformation, transformation);
        auto computed_tr = DateTimeRangeHelper::computeTransformation(range1,range2).value();
        QCOMPARE(computed_tr, transformation);
        QCOMPARE(range1.transform(transformation),range2);
        QCOMPARE(range1.transform(computed_tr),range2);
    }

    void testRangeDiff_data()
    {
        QTest::addColumn<DateTimeRange>("source");
        QTest::addColumn<DateTimeRange>("destination");
        QTest::addColumn<int>("count");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("Same range") << range << range << 0;
        QTest::newRow("No missing data (zoom in)") << range << range*0.9 << 0;
        QTest::newRow("Missing data on both sides (zoom out)") << range << range*2. << 2;
        QTest::newRow("Missing data on left side (pan left)") << range << range-Seconds<double>{1000.} << 1;
        QTest::newRow("Missing data on right side (pan right)") << range << range+Seconds<double>{1000.} << 1;
        QTest::newRow("Missing data on right side no intersect (big pan right)") << range << range+Seconds<double>{24.*60.*60.*2} << 1;
        QTest::newRow("Missing data on left side no intersect (big pan left)") << range << range-Seconds<double>{24.*60.*60.*2} << 1;

    }

    void testRangeDiff()
    {
        QFETCH(DateTimeRange,source);
        QFETCH(DateTimeRange,destination);
        QFETCH(int, count);
        auto diff = destination-source;
        QCOMPARE(diff.size(),count);
    }
};
QTEST_MAIN(TestDateTimeRange)


#include "TestDateTimeRange.moc"
