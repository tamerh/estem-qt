#ifndef TESTCOMMUNICATOR_H
#define TEST_COMMUNICATOR_H

#include <QtTest/QtTest>
#include <QtCore/QDebug>

//#include "serialcommunicator.h"
#include "applicationcontroller.h"

class SerialCommunicator;

class TestCommunicator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void decodeSimpleValidSequence();
    void decodeValidSequenceWithLeadingJunk();
    void decodeValidSequenceWithEscapes();
    void decodeJunk();
    void decodeFragmentedMessage();
    void decodeSeveralMessages();
    void decodeUnknownMessage();

    void valveChange();
    void pumpChange();
    void pressureChange();

    void frameMessage();

    void uptime();

    void parseDecodedBuffer();
    // To do:
    // void error();

private:

    SerialCommunicator * c;
};


class CommunicatorMockApplicationController : public ApplicationController
{
public:
    CommunicatorMockApplicationController() {}
};

#endif
