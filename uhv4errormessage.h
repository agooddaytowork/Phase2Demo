#ifndef UHV4ERRORMESSAGE_H
#define UHV4ERRORMESSAGE_H

struct UHV4ErrorMessage {
    int addr = 0;
    int CH = 0;
    bool UHV4_InterLock_error = false;
    bool UHV4_OverTemp_HV_error = false;
    bool UHV4_Protect_error = false;
    bool UHV4_Measurement_error = false;
    bool UHV4_HV_out_error = false;
    bool UHV4_Short_circuit_error = false;
    bool UHV4_HV_disable_error = false;
    bool UHV4_PowerInput_HV_error = false;
    bool UHV4_PowerInput_PFC_error = false;
};

#endif // UHV4ERRORMESSAGE_H
