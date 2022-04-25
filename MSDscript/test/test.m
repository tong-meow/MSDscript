//
//  test.m
//  test
//
//  Created by Tong Shen on 1/24/22.
//
#import <XCTest/XCTest.h>
#include "run.hpp"

@interface test : XCTestCase
@end

@implementation test
- (void)testAll {
  if (!run_tests())
    XCTFail(@"failed");
}
@end
