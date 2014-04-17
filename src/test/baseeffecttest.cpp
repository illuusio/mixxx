#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/baseeffecttest.h"

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

void BaseEffectTest::registerTestEffect(const EffectManifest& manifest) {
    MockEffectProcessor* pProcessor = new MockEffectProcessor();
    MockEffectInstantiator* pInstantiator = new MockEffectInstantiator();

    EXPECT_CALL(*pInstantiator, instantiate(_, _))
            .Times(1)
            .WillOnce(Return(pProcessor));

    m_pTestBackend->registerEffect(manifest.id(), manifest,
                                   EffectInstantiatorPointer(pInstantiator));
}
