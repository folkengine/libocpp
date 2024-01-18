#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
namespace fs = std::filesystem;

#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <optional>

namespace ocpp {
namespace v16 {

/**
 * Two paths:
 * 1. Continue testing the branches within ValidateProfile
 * 2. Begin creating the tests for SmartChargingHandler::clear_all_profiles_with_filter() 
*/
class ChargepointTestFixture : public testing::Test {
protected:
    void SetUp() override {
    }

    ChargingSchedule createChargeSchedule() {
        return ChargingSchedule{{}};
    }

    ChargingSchedule createChargeSchedule(ChargingRateUnit chargingRateUnit) {
        std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
        std::optional<int32_t> duration;
        std::optional<ocpp::DateTime> startSchedule;
        std::optional<float> minChargingRate;

        return ChargingSchedule{chargingRateUnit, chargingSchedulePeriod, duration, startSchedule, minChargingRate};
    }

    ChargingProfile createChargingProfile(ChargingSchedule chargingSchedule) {
        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{
            chargingProfileId,
            stackLevel,
            chargingProfilePurpose,
            chargingProfileKind,
            chargingSchedule,
            {},                         // transactionId
            recurrencyKind,
            {},                         // validFrom
            {}                          // validTo
        };
    }

    /**
     * TxDefaultProfile, stack #1: time-of-day limitation to 2 kW, recurring every day from 17:00h to 20:00h.
     */
    ChargingProfile createChargingProfile_Example1() {
        auto chargingRateUnit = ChargingRateUnit::W;
        auto chargingSchedulePeriod = std::vector<ChargingSchedulePeriod>{ChargingSchedulePeriod{0, 2000, 1}};
        auto duration = 1080;
        auto startSchedule = ocpp::DateTime("2024-01-17T17:00:00");
        float minChargingRate = 0;
        auto chargingSchedule = ChargingSchedule{
            chargingRateUnit,
            chargingSchedulePeriod,
            duration,
            startSchedule,
            minChargingRate
        };

        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{
            chargingProfileId,
            stackLevel,
            chargingProfilePurpose,
            chargingProfileKind,
            chargingSchedule,
            {},                         // transactionId
            recurrencyKind,
            {},                         // validFrom
            {}                          // validTo
        };
    }

    /**
     * TxDefaultProfile, stack #2: overruling Sundays to no limit, recurring every week starting 2020-01-05.
     */
    ChargingProfile createChargingProfile_Example2() {
        auto chargingRateUnit = ChargingRateUnit::W;
        auto chargingSchedulePeriod = std::vector<ChargingSchedulePeriod>{ChargingSchedulePeriod{0, 999999, 1}};
        auto duration = 0;
        auto startSchedule = ocpp::DateTime("2020-01-19T00:00:00");
        float minChargingRate = 0;
        auto chargingSchedule = ChargingSchedule{
            chargingRateUnit,
            chargingSchedulePeriod,
            duration,
            startSchedule,
            minChargingRate
        };

        auto chargingProfileId = 11;
        auto stackLevel = 2;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Recurring;
        auto recurrencyKind = RecurrencyKindType::Weekly;
        return ChargingProfile{
            chargingProfileId,
            stackLevel,
            chargingProfilePurpose,
            chargingProfileKind,
            chargingSchedule,
            {},                         // transactionId
            recurrencyKind,
            {},
            {}
        };
    }

    SmartChargingHandler* createSmartChargingHandler() {
        auto c1 = std::make_shared<Connector>(Connector{1});
        connectors[1] = c1;
        auto handler = new SmartChargingHandler(connectors, database_handler, true);
        return handler;
    }

    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<DatabaseHandler> database_handler;

    const int connector_id = 1;
    bool ignore_no_transaction = true;
    const int profile_max_stack_level = 1;
    const int max_charging_profiles_installed = 1;
    const int charging_schedule_max_periods = 1;
};

/**
 * PB01 Valid Profile
 */
TEST_F(ChargepointTestFixture, ValidateProfile) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    // const int connector_id = -1;
    // bool ignore_no_transaction = true;
    // const int profile_max_stack_level = 1;
    // const int max_charging_profiles_installed = 1;
    // const int charging_schedule_max_periods = 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * PB01 Valid Profile: Example 1
 * 
 * This example is taken from the OCPP 2.0.1 Spec page. 241
 */
TEST_F(ChargepointTestFixture, ValidateProfile__example1) {
    // GTEST_SKIP();
    auto profile = createChargingProfile_Example1();
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::W};
    auto handler = createSmartChargingHandler();

    // const int connector_id = -1;
    // bool ignore_no_transaction = true;
    // const int profile_max_stack_level = 1;
    // const int max_charging_profiles_installed = 1;
    // const int charging_schedule_max_periods = 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * PB01 Valid Profile: Example 2
 * 
 * This example is taken from the OCPP 2.0.1 Spec page. 241
 */
TEST_F(ChargepointTestFixture, ValidateProfile__example2) {
    GTEST_SKIP();
    auto profile = createChargingProfile_Example2();
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    // const int connector_id = -1;
    // bool ignore_no_transaction = true;
    // const int profile_max_stack_level = 1;
    // const int max_charging_profiles_installed = 1;
    // const int charging_schedule_max_periods = 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB01 Valid Profile, ConnectorID gt this->connectors.size()
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ConnectorIdGreaterThanConnectorsSize__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    const int connector_id = INT_MAX;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB02 Valid Profile, ConnectorID lt 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_NegativeConnectorIdTest__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();
    
    const int connector_id = -1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB03 profile.stackLevel lt 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_ConnectorIdZero__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.stackLevel = -1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB04 profile.stackLevel gt this->profile_max_stack_level
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_StackLevelGreaterThanMaxStackLevel__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.stackLevel = profile_max_stack_level + 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB05 profile.chargingProfileKind == Absolute && !profile.chargingSchedule.startSchedule 
*/
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_ChargingProfileKindAbsoluteNoStartSchedule__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    // Create a SmartChargingHandler where allow_charging_profile_without_start_schedule is set to false
    auto c1 = std::make_shared<Connector>(Connector{1});
    connectors[1] = c1;
    auto allow_charging_profile_without_start_schedule = false;
    auto handler = new SmartChargingHandler(connectors, database_handler, allow_charging_profile_without_start_schedule);

    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);
    
    ASSERT_FALSE(sut);
}

} // namespace v16
} // namespace ocpp