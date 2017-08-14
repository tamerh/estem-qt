#ifndef CONSTANTS_H
#define CONSTANTS_H

// The following are constants for the serial communicator. Not necessarily for the whole program.
// TODO: possibly move this to within the Communicator class

#define PR_MAX_VALUE UINT8_MAX

// this enum could also be split into valves/prs/pumps
enum Component : uint8_t {
    VALVE1 = 0,
    VALVE2,
    VALVE3,
    VALVE4,
    VALVE5,
    VALVE6,
    VALVE7,
    VALVE8,
    VALVE9,
    VALVE10,
    VALVE11,
    VALVE12,
    VALVE13,
    VALVE14,
    VALVE15,
    VALVE16,
    VALVE17,
    VALVE18,
    VALVE19,
    VALVE20,
    VALVE21,
    VALVE22,
    VALVE23,
    VALVE24,
    VALVE25,
    VALVE26,
    VALVE27,
    VALVE28,
    VALVE29,
    VALVE30,
    VALVE31,
    VALVE32,
    PR1,
    PR2,
    PR3,
    PUMP1,
    PUMP2,
    ALL_COMPONENTS,
    COMPONENTS_END // leave this as the last element, to allow iterating over the enum
};

enum ValveStates : uint8_t {
    OPEN = COMPONENTS_END+1,
    CLOSED
};

enum PumpStates : uint8_t {
    ON = CLOSED+1,
    OFF
};

enum SerialRequests : uint8_t {
    STATUS = OFF+1// request status of a certain component (or all)
};

//
// ---------------------------------------
//

#endif