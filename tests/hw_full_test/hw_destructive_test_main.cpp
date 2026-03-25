// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "hw_test_main_common.hpp"

int main(int argc, char **argv) {
    hw_test_main::enableDestructiveMode();
    ::testing::InitGoogleTest(&argc, argv);
    hw_test_main::setDefaultFilterIfUnset(hw_test_main::kDestructiveTestFilter);
    ::testing::AddGlobalTestEnvironment(new hw_test_main::FirmwareUpgradeEnvironment());
    return RUN_ALL_TESTS();
}