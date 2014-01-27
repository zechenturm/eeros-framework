#include "MySafetyProperties.hpp"
#include "MyControlSystem.hpp"

#include <eeros/hal/HAL.hpp>
#include <eeros/safety/inputActions.hpp>
#include <eeros/safety/OutputAction.hpp>

#include <unistd.h>
#include <iostream>
#include <vector>
#include <initializer_list>

using namespace eeros;
using namespace eeros::hal;
using namespace eeros::safety;

MySafetyProperties::MySafetyProperties() {
	
	HAL& hal = HAL::instance();

	// ############ Define critical outputs ############
	enable = hal.getLogicSystemOutput("enable");
	
	criticalOutputs = { enable };
	
	// ############ Define critical inputs ############
	emergency = hal.getLogicSystemInput("emergency");
	q = hal.getRealSystemInput("q");
	
	criticalInputs = { emergency, q };
	
	// ############ Define Levels ############
	levels = {		
			{ off,                        "Software is off",                                     },
			{ emergencyState,             "Emergency state",                                     },
			{ systemOn,                   "System is ready, power off",                          },
			{ startingControl,            "System is stopping controller",                       },
			{ stoppingControl,            "System is starting controller",                       },
			{ powerOn,                    "Power is on, motors are controlled",                  },
			{ moving,                     "System is moving",                                    }
		};
		
		// ############ Add events to the levels ############
		level(off                        ).addEvent(doSystemOn,                     systemOn,                   kPublicEvent  );
		level(systemOn                   ).addEvent(startControl,                   startingControl,            kPublicEvent  );
		level(systemOn                   ).addEvent(doSystemOff,                    off,                        kPublicEvent  );
		level(startingControl            ).addEvent(startControlDone,               powerOn,                    kPrivateEvent );
		level(stoppingControl            ).addEvent(stopControlDone,                systemOn,                   kPrivateEvent );
		level(powerOn                    ).addEvent(startMoving,                    moving,                     kPublicEvent  );
		level(powerOn                    ).addEvent(stopControl,                    powerOn,                    kPublicEvent  );
		level(moving                     ).addEvent(stopMoving,                     powerOn,                    kPublicEvent  );
		level(emergencyState             ).addEvent(resetEmergency,                 systemOn,                   kPublicEvent  );
		
		// Add events to multiple levels
		addEventToLevelAndAbove(systemOn, doEmergency, emergencyState, kPublicEvent);
		
		// ############ Define input states and events for all levels ############
		level(off                        ).setInputActions( { ignore(emergency) });
		level(emergencyState             ).setInputActions( { ignore(emergency) });
		level(systemOn                   ).setInputActions( { check(emergency, true , doEmergency) });
		level(startingControl            ).setInputActions( { check(emergency, true , doEmergency) });
		level(stoppingControl            ).setInputActions( { check(emergency, true , doEmergency) });
		level(powerOn                    ).setInputActions( { check(emergency, true , doEmergency) });
		level(moving                     ).setInputActions( { check(emergency, true , doEmergency) });
		
		// Define and add level functions
		level(off).setLevelAction([&](SafetyContext* privateContext) {
			privateContext->triggerEvent(systemOn);
		});
		
		level(startingControl).setLevelAction([&](SafetyContext* privateContext) {
			MyControlSystem::instance().start();
		});
		
		level(stoppingControl).setLevelAction([&](SafetyContext* privateContext) {
			MyControlSystem::instance().stop();
		});
		
		// Define entry level
		entryLevel = off;
}

MySafetyProperties::~MySafetyProperties() {
	// nothing to do
}