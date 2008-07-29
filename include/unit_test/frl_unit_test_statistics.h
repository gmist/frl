#ifndef frl_unit_test_statistics_h_
#define frl_unit_test_statistics_h_

namespace frl{ namespace unit_test{

// Unit testing statistic structure
struct UnitTestStatistics
{
	unsigned int testsNumber;		// Number all worked tests
	unsigned int okTestsNumber;	// Number success worked test
	unsigned int failTestsNumber;	// Number fail worked tests
}; // struct UnitTestStatistic

} // namespace unit_test
}// FatRat Library

#endif // frl_unit_test_statistics_h_
