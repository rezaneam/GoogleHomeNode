#include <bsec.h>
#include <BME280.h>

class EnvironmentSensor
{
public:
    bool Initialize();
    bool TakeSample();
    float readTemperature();
    float readPressure();
    float readHumidity();
    float readAirQuality();

private:
    float temperature;
    float pressure;
    float humidity;
    float airQuality;

    bool isBME680;
    bool isBME280;
    bool isBMP280;

    BME280 bmx280;
    Bsec bme680;

    bsec_virtual_sensor_t sensorList[10] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    void initializeBMx280();
    void initializeBME680();
};