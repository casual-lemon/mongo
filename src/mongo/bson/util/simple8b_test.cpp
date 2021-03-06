/**
 *    Copyright (C) 2021-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include <vector>

#include "mongo/bson/util/simple8b.h"
#include "mongo/unittest/unittest.h"

using namespace mongo;

void assertVectorsEqual(const std::vector<uint64_t>& actualVector,
                        const std::vector<uint64_t>& expectedVector) {
    ASSERT_EQ(actualVector.size(), expectedVector.size());

    for (size_t i = 0; i < actualVector.size(); ++i) {
        ASSERT_EQ(actualVector[i], expectedVector[i]);
    }
}

TEST(Simple8b, EncodeOneValue) {
    uint8_t selector = 14;
    std::vector<uint64_t> values = {1};
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0x0000000000000001E);
}

TEST(Simple8b, DecodeOneValue) {
    uint64_t simple8bWord = 0x0000000000000001E;
    std::vector<uint64_t> values = Simple8b::decodeSimple8b(simple8bWord);
    std::vector<uint64_t> expectedValues = {1};
    assertVectorsEqual(values, expectedValues);
}

TEST(Simple8b, EncodeMultipleValues) {
    uint8_t selector = 12;
    std::vector<uint64_t> values = {1, 2, 3};
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0x000030000200001C);
}

TEST(Simple8b, DecodeMultipleValues) {
    uint64_t simple8bWord = 0x000030000200001C;
    std::vector<uint64_t> values = Simple8b::decodeSimple8b(simple8bWord);
    std::vector<uint64_t> expectedValues = {1, 2, 3};
    assertVectorsEqual(values, expectedValues);
}

TEST(Simple8b, EncodeMaxValues) {
    uint8_t selector = 1;
    std::vector<uint64_t> values(60, 1);
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0xFFFFFFFFFFFFFFF1);
}

TEST(Simple8b, DecodeMaxValues) {
    uint64_t simple8bWord = 0xFFFFFFFFFFFFFFF1;
    std::vector<uint64_t> values = Simple8b::decodeSimple8b(simple8bWord);
    std::vector<uint64_t> expectedValues(60, 1);
    assertVectorsEqual(values, expectedValues);
}

TEST(Simple8b, EncodeWithTrailingDirtyBits) {
    uint8_t selector = 8;
    std::vector<uint64_t> values(7, 1);
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0x0010101010101018);
}

TEST(Simple8b, DecodeWithTrailingDirtyBits) {
    uint64_t simple8bWord = 0x0010101010101018;
    std::vector<uint64_t> values = Simple8b::decodeSimple8b(simple8bWord);
    std::vector<uint64_t> expectedValues(7, 1);
    assertVectorsEqual(values, expectedValues);
}

TEST(Simple8b, InvalidEncodeZeroSelector) {
    uint8_t selector = 0;
    std::vector<uint64_t> values = {};
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0x0000000000000000);
}

TEST(Simple8b, InvalidEncodeFifteenSelector) {
    uint8_t selector = 15;
    std::vector<uint64_t> values = {};
    uint64_t simple8bWord = Simple8b::encodeSimple8b(selector, values);
    ASSERT_EQUALS(simple8bWord, 0x0000000000000000);
}

TEST(Simple8b, InvalidDecodeZeroSelector) {
    uint64_t simple8bWord = 0x0000000000000000;
    std::vector<uint64_t> values = Simple8b::decodeSimple8b(simple8bWord);
    ASSERT_EQUALS(values.size(), 0);
}
