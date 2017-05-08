#ifndef LIGHTSOURCE_RPC_H
#define LIGHTSOURCE_RPC_H

#include "LightsourceStrips.h"
#include "Lightsource.h"

class LightsourceRpc
{
	public:
		LightsourceRpc(){}
		~LightsourceRpc();
		void begin();
		void process(ESP8266WebServer &srv);
		String init( JsonObject &request);
		String testConfig( JsonObject &request);
		String writeConfig( JsonObject &request);
	private:
		LightsourceStrips *lightsourceStrips;
};
#endif
