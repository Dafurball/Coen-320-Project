

#ifndef SRC_ATC_H_
#define SRC_ATC_H_


#include "Radar.h"
#include "ComputerSystem.h"
#include "OperatorConsole.h"
#include "DataDisplay.h"
#include "CommunicationSystem.h"

class ATC {


public:
	ATC();
	virtual ~ATC();

private:

	Radar radar;
	ComputerSystem computerSystem;
	OperatorConsole operatorConsole;
	DataDisplay dataDisplay;
	CommunicationSystem communicationSystem;


};

#endif
