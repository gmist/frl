#ifndef FRL_UNIT_TEST_STATISTICS_H_
#define FRL_UNIT_TEST_STATISTICS_H_

namespace frl
{
namespace unit_test
{
// Unit testing statistic structure
struct UnitTestStatistics
{
	unsigned int testsNumber;		// Number all worked tests
	unsigned int okTestsNumber;	// Number success worked test
	unsigned int failTestsNumber;	// Number fail worked tests
}; // struct UnitTestStatistic
};
}// FatRat Library

#endif /*FRL_UNIT_TEST_STATISTICS_H_*/
