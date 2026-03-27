// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "../test_common.hpp"

extern void parsePerfArgs(int argc, char **argv);

int main(int argc, char **argv) {
    parsePerfArgs(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
