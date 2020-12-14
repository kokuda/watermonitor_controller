
// One time initialization of the library.
// Should be called from setup()
void adafruitMqttSetup();

// Connect to io.adafruit.com or indicate connection status.
// Returns true if connected or already connected.
bool adafruitMqttConnect();

// Publish the stats to the appropriate feeds.
void adafruitMqttPublish(float distance, float temperature, float humidity);

// Download and process incoming data for as long as specified.
void adafruitMqttProcessPackets(uint32_t defaultInterval);
