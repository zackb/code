import XCTest

import neuroTests

var tests = [XCTestCaseEntry]()
tests += neuroTests.allTests()
XCTMain(tests)
