#include "communicator.h"
#include "applicationcontroller.h"


Communicator::Communicator(ApplicationController* applicationController)
    : mConnectionStatus(Disconnected)
    , appController(applicationController)
{
}

Communicator::~Communicator()
{
}

Communicator::ConnectionStatus Communicator::getConnectionStatus() const
{
    return mConnectionStatus;
}

/**
 * @brief Return the connection status in human-readable form
 */
QString Communicator::getConnectionStatusString() const
{
    switch (mConnectionStatus) {
        case Disconnected:
            return "Disconnected";
        case Connecting:
            return "Connecting";
        case Connected:
            return "Connected";
        default:
            return QString();
    }
}

/**
 * @brief Open or close a specific valve
 * @param valveNumber The valve number
 * @param open If true, valve will be opened; otherwise, valve will be closed
 */
void Communicator::setValve(uint valveNumber, bool open)
{
    qDebug() << "Communicator: setting valve" << valveNumber << (open ? "open" : "closed");

    QByteArray message;
    message.push_back(VALVE);
    message.push_back(1);
    message.push_back((uint8_t)valveNumber);
    message.push_back(1);
    message.push_back((uint8_t)open);

    sendMessage(frameMessage(message));
}

/**
 * @brief Switch a given pump on or off.
 * @param pumpNumber The pump number
 * @param on If true, the pump will be turned on; otherwise, the pump will be turned off.
 */
void Communicator::setPump(uint pumpNumber, bool on)
{
    qDebug() << "Communicator: setting pump" << pumpNumber << (on ? "on" : "off");

    QByteArray message;
    message.push_back(PUMP);
    message.push_back(1);
    message.push_back((uint8_t)pumpNumber);
    message.push_back(1);
    message.push_back((uint8_t)on);

    sendMessage(frameMessage(message));
}

/**
 * @brief Set the pressure setpoint of a given controller
 * @param controllerNumber The controller number
 * @param pressure A double between 0 and 1.0, with 0 being the minimum and 1 being the maximum pressures allowed by the controller
 */
void Communicator::setPressure(uint controllerNumber, double pressure)
{
    qDebug() << "Communicator: setting pressure controller" << controllerNumber << " to " << pressure;

    if (pressure < 0. || pressure > 1.) {
        qWarning() << "Pressure invalid. Must be between 0 and 1.";
        return;
    }
    uint8_t sp = pressure*PR_MAX_VALUE;

    QByteArray message;
    message.push_back(PRESSURE);
    message.push_back(1);
    message.push_back((uint8_t)controllerNumber);
    message.push_back(1);
    message.push_back(sp);

    sendMessage(frameMessage(message));
}

/**
 * @brief Request status of all components
 */
void Communicator::requestStatus()
{
    qDebug() << "Communicator: requesting status of all components";
    QByteArray message;
    message.push_back(STATUS);
    sendMessage(frameMessage(message));
}

/**
 * @brief Frame a message, i.e. add start and stop bytes, and escapes
 * @param message The message to be framed
 * @return The framed message, ready to send with sendMessage(QByteArray)
 */
QByteArray Communicator::frameMessage(QByteArray message)
{
    QByteArray framedMessage;
    framedMessage.push_back(START_BYTE);

    for (uint8_t c : message) {
        if (c == STOP_BYTE || c == ESCAPE_BYTE)
            framedMessage.push_back(ESCAPE_BYTE);
        framedMessage.push_back(c);
    }
    framedMessage.push_back(STOP_BYTE);

    return framedMessage;
}

/**
 * @brief Display a log message received from the microcontroller
 * @param level How bad it is
 * @param message The message that was received
 *
 * The message is passed to qCritical, qWarning etc. based on its level
 */
void Communicator::logMicrocontrollerMessage(LogLevel level, const QByteArray &message)
{
    switch (level) {
        case LOG_FATAL:
        case LOG_ERROR:
            qCritical().noquote() << "Microcontroller: " << message;
            break;
        case LOG_WARNING:
            qWarning().noquote() << "Microcontroller: " << message;
            break;
        case LOG_INFO:
            qInfo().noquote() << "Microcontroller: " << message;
            break;
        case LOG_DEBUG:
            qDebug().noquote() << "Microcontroller: " << message;
            break;
        default:
            qDebug().noquote() << "Message from microcontroller with unknown level:" << message;
            break;
    }
}

/**
 * @brief Parse the buffer to remove escape characters, start and stop bytes
 * @returns The first valid message found (or an empty QByteArray if no valid message is found)
 *
 * This method should be called whenever new data arrives on the serial buffer (mBuffer).
 *
 * If a start byte is found, but no valid end byte is found, this method returns an
 * empty buffer; when it is next called, it will continue where it left off.
 *
 * Since the portion of mBuffer that was successfully parsed is erased, this method
 * can be called repeatedly as long as there is data left in mBuffer.
 *
 * The parseDecodedBuffer method should be called when this method returns data.
 */
QByteArray Communicator::decodeBuffer()
{
    // Data is framed with a start and end byte, and can contain
    // escape bytes (to escape a stop byte or another escape byte)

    // Data is effectively moved from mBuffer to mDecodedBuffer, minus
    // start bytes, escape bytes and stop bytes. Any data preceding a start
    // byte is discarded. When a valid message (i.e. any data framed by a start
    // and end byte) is found, it is returned.

    bool foundCompleteMessage(false);
    int decoderIndex(0);

    for (uint8_t c: mBuffer) {
        if (mDecoderRecording) {
            if (mDecoderEscaped) {
                mDecodedBuffer.append(c);
                mDecoderEscaped = false;
            }
            else if (c == ESCAPE_BYTE)
                mDecoderEscaped = true;
            else if (c == STOP_BYTE) {
                foundCompleteMessage = true;
                mDecoderRecording = false;
                break;
            }
            else if (mLastByteWasStart && c >= NUM_COMMANDS) {
                // Invalid command, we stop right there
                mDecoderRecording = false;
                mLastByteWasStart = false;
                break;
            }
            else
                mDecodedBuffer.append(c);
            mLastByteWasStart = false;
        }
        else if (c == START_BYTE) {
            mDecoderRecording = true;
            mLastByteWasStart = true;
        }
        decoderIndex++;
    }
    // Everything that was parsed already should be removed from mBuffer
    mBuffer.remove(0, decoderIndex);

    if (foundCompleteMessage) {
        QByteArray decodedBuffer = mDecodedBuffer;
        mDecodedBuffer.clear();
        return decodedBuffer;
    }
    return QByteArray();
}

/**
 * @brief Parse the decoded message buffer and call handleCommand for each command found
 *
 * The buffer is cleared after use.
 */
void Communicator::parseDecodedBuffer(QByteArray buffer)
{
    // Messages have the format:
    //     command parameter_size param_data [param_size] [param_data] ....
    // With one or more parameters.

    if (buffer.size() < 2) {
        qWarning() << "parseDecodedBuffer called when the buffer is too short to contain a message";
        return;
    }

    QList<QByteArray> parameters;

    uint8_t command = buffer[0];


    if (command < NUM_COMMANDS) {
        int i(1);

        while (i < buffer.size()) {
            uint8_t paramSize = buffer[i];
            i++;
            if (i + paramSize <= buffer.size()) {
                QByteArray paramData = buffer.mid(i, paramSize);
                parameters.push_back(paramData);
            }
            else {
                qWarning() << "Command parameter incomplete; ignoring command";
                return;
            }
            i += paramSize;
        }
        handleCommand(command, parameters);
    }
    else
        qDebug() << "Unknown command received. Full buffer: " << buffer;
}

/**
 * @brief Handle a command received from the microcontroller, passing it on higher
 * @param command The command, e.g. PUMP, VALVE,...
 * @param parameters A list with each item being a parameter, represented by a QByteArray
 *
 * This function emits signals based on the commands received, e.g. calling valveStateChanged
 * when a valid VALVE command is received. Incorrect commands trigger an error message.
 */
void Communicator::handleCommand(uint8_t command, QList<QByteArray> parameters)
{
    int nParameters = parameters.size();

    switch (command) {
        case VALVE:
            // Should have 2 one-byte parameters: valve number and valve state.
            // State is 0 (closed) or 1 (open)
            if (nParameters != 2)
                qWarning() << "Invalid number of parameters for VALVE command:" << nParameters;
            else if (parameters[0].length() != 1 || parameters[1].length() != 1)
                qWarning() << "Invalid parameter sizes for VALVE command";
            else
                emit valveStateChanged((uint8_t)parameters[0][0], (bool)parameters[1][0]);
            break;

        case PUMP:
            // Should have 2 one-byte parameters: number and state (0 (off) or 1 (on))
            if (nParameters != 2)
                qWarning() << "Invalid number of parameters for PUMP command:" << nParameters;
            else if (parameters[0].length() != 1 || parameters[1].length() != 1)
                qWarning() << "Invalid parameter sizes for PUMP command";
            else
                emit pumpStateChanged((uint8_t)parameters[0][0], (bool)parameters[1][0]);
            break;

        case PRESSURE:
            // Should have 3 one-byte parameters: number, setpoint and measured value
            if (nParameters != 3)
                qWarning() << "Invalid number of parameters for PRESSURE command:" << nParameters;
            else if (parameters[0].length() != 1 || parameters[1].length() != 1 || parameters[2].length() != 1)
                qWarning() << "Invalid parameter sizes for PRESSURE command";
            else {
                uint8_t number = parameters[0][0];
                uint8_t sp = parameters[1][0];
                uint8_t pv = parameters[2][0];

                emit pressureSetpointChanged(number, double(sp)/PR_MAX_VALUE);
                emit pressureChanged(number, double(pv)/PR_MAX_VALUE);
            }
            break;

        case UPTIME:
            // Should have one 4-byte parameter
            if (nParameters != 1)
                qWarning() << "Invalid number of parameters for UPTIME command:" << nParameters;
            else if (parameters[0].length() != 4)
                qWarning() << "Invalid parameter size for UPTIME command";
            else {
                uint8_t value0 = parameters[0][0];
                uint8_t value1 = parameters[0][1];
                uint8_t value2 = parameters[0][2];
                uint8_t value3 = parameters[0][3];
                long uptime = value0 << 24 | value1 << 16 | value2 << 8 | value3;

                emit uptimeChanged(uptime);
            }
            break;

        case ERROR:
            qDebug() << "Error received";
            break;

        case LOG:
            if (nParameters != 2)
                qWarning() << "Invalid number of parameters for LOG command" << nParameters;
            else
                logMicrocontrollerMessage(LogLevel((uint8_t)parameters[0][0]), parameters[1]);
            break;
        default:
            qWarning() << "Unknown command received:" << int(command);
            break;
    }

}



void Communicator::setConnectionStatus(ConnectionStatus status)
{
    if (status != mConnectionStatus) {
        mConnectionStatus = status;
        emit connectionStatusChanged(status);
    }
}
