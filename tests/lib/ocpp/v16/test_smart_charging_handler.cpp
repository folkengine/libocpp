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
        return ChargingProfile{1,
                                   1,
                                   ChargingProfilePurposeType::TxDefaultProfile,
                                   ChargingProfileKindType::Absolute,
                                   chargingSchedule,
                                   1,
                                   RecurrencyKindType::Daily,
                                   {},
                                   {}};
    }

    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<DatabaseHandler> database_handler;
};

TEST_F(ChargepointTestFixture, validateNoRealProfilesTest) {

    auto chargingSchedule = createChargeSchedule();

    auto profile = createChargingProfile(chargingSchedule);

    const int connector_id = 1;
    bool ignore_no_transaction = true;
    const int profile_max_stack_level = 1;
    const int max_charging_profiles_installed = 1;
    const int charging_schedule_max_periods = 1;
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{};

    auto handler =
        new SmartChargingHandler(connectors, database_handler, false);
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);
    ASSERT_FALSE(sut);
}

TEST_F(ChargepointTestFixture, validateProfileTest) {
    auto c1 = std::make_shared<Connector>(Connector{1});
    connectors[1] = c1;

    auto chargingSchedule = createChargeSchedule(ChargingRateUnit::A);

    auto profile = createChargingProfile(chargingSchedule);

    const int connector_id = 1;
    bool ignore_no_transaction = true;
    const int profile_max_stack_level = 1;
    const int max_charging_profiles_installed = 1;
    const int charging_schedule_max_periods = 1;
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    auto handler =
        new SmartChargingHandler(connectors, database_handler, true);
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

} // namespace v16
} // namespace ocpp