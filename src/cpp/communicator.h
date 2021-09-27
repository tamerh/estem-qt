#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QtCore>

#include "constants.h"

class ApplicationController;


/**
 * @brief The Communicator class provides an interface to the microcontroller
 *
 * Communicator is an abstract class, with SerialCommunicator and BluetoothCommunicator handling
 * the specifics related to USB and Bluetooth communication.
 *
 * In both cases, the microcontroller is automatically detected; a call to `connect()` is all that
 * is necessary to connect to it.
 * Connection status can be checked using the getConnectionStatus() and getConnectionStatusString()
 * functions, or better, by connecting to the connectionStatusChanged signal.
 *
 * The interface to the actual functionality of the microcontroller is provided by the setValve,
 * setPump, setPressure, and requestStatus functions.
 * The first three tell the microcontroller to do something, e.g toggle a valve, while the
 * requestStatus function requests an update of all components' statuses.
 *
 * The signals valveStateChanged, pumpStateChanged, pressureChanged and pressureSetpointChanged
 *  are emitted whenever the microcontroller communicates the current status of a component.
 * Connect to these to know the current status of the hardware.
 *
 * In order to know how many components are available, and what pressures are supported by the pressure controllers,
 * use the nValves, nPumps, nPressureControllers, minPressure and maxPressure functions.
 *
 * Valves, pumps and pressure controllers are 1-indexed. I.e valveNumber will be between 1 and 32;
 * pumpNumber between 1 and 2; controllerNumber between 1 and 3.
 *
 * Commands exchanged between the host and microcontroller have the following format:
 *
 *   Start byte | Command [1 Byte] | Param size [1B] | Param data [nB] | [...] | Stop byte
 *
 * Param size and param data can be repeated if the command needs several parameters.
 *
 * On the decoding side, messages are received by whatever mechanism the subclasses
 * (Serial/BluetoothCommunicator) uses; they are added to mBuffer, then the following methods
 * are called: decodeBuffer -> parseDecodedBuffer -> handleCommand.
 *
 */
class Communicator : public QObject
{
    Q_OBJECT

public:
    enum ConnectionStatus {
        Disconnected,
        Connecting,
        Connected
    };

    Communicator(ApplicationController* applicationController);
    virtual ~Communicator ();

    ConnectionStatus getConnectionStatus() const;
    QString getConnectionStatusString() const;


public slots:
    virtual void connect() = 0;
    void setValve(uint valveNumber, bool open);
    void setPressure(uint controllerNumber, double pressure);
    void setPump(uint pumpNumber, bool on);
    void requestStatus();

signals:
    void valveStateChanged(uint valveNumber, bool open);
    void pumpStateChanged(uint pumpNumber, bool on);
    void pressureChanged(uint controllerNumber, double pressure);
    void pressureSetpointChanged(uint controllerNumber, double pressure);
    void uptimeChanged(ulong seconds);

    void connectionStatusChanged(ConnectionStatus newStatus);

protected:
    void setConnectionStatus(ConnectionStatus status);
    QByteArray frameMessage(QByteArray message);
    virtual void sendMessage(QByteArray message) = 0;
    void logMicrocontrollerMessage(LogLevel level, QByteArray const& message);

    ConnectionStatus mConnectionStatus;

    /// The buffer of incoming data, populated by the serial port backend
    QByteArray mBuffer;

    // Message parser-related members
    QByteArray decodeBuffer();
    void parseDecodedBuffer(QByteArray buffer);
    void handleCommand(uint8_t command, QList<QByteArray> parameters);

    QByteArray mDecodedBuffer;
    bool mDecoderRecording;
    bool mDecoderEscaped;
    bool mLastByteWasStart;

    ApplicationController* appController;

#ifdef TESTING
    friend class TestCommunicator;
#endif

};

#endif
