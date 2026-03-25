// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "hw_test_main_common.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    hw_test_main::setDefaultFilterIfUnset(std::string("*-") + hw_test_main::kDestructiveTestFilter);
    return RUN_ALL_TESTS();
}