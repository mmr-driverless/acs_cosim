#ifndef CAR_CONTROLS_H
#define CAR_CONTROLS_H

class CarControls {
public:
    bool gearUp;
    bool gearDn;
    bool drs;
    bool kers;
    bool brakeBalanceUp;
    bool brakeBalanceDn;
    int requestedGearIndex;
    bool isShifterSupported;
    float handBrake;
    bool absUp;
    bool absDn;
    bool tcUp;
    bool tcDn;
    bool turboUp;
    bool turboDn;
    bool engineBrakeUp;
    bool engineBrakeDn;
    bool MGUKDeliveryUp;
    bool MGUKDeliveryDn;
    bool MGUKRecoveryUp;
    bool MGUKRecoveryDn;
    bool MGUHMode;
    float gas;
    float brake;
    float steer;
    float clutch;
};

#endif // CAR_CONTROLS_H

