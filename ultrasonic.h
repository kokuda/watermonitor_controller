
// Returns the distance travelled by sound in ping_us microseconds at an air temperature of temperature
float getDistance(unsigned int ping_us, float temperature);

// The distance sensor sometimes returns very short distances.
// This will ignore any values that are too different between calls and return the previous value
// until the data is valid
float validateDistance(float distance, unsigned long nowMs);

// Returns the ping time in microseconds from the ultrasonic sensor
unsigned int getUltrasonicSensorPingTimeUs();
