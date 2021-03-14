#include <bsec.h>
#include <BME280.h>

typedef struct
{
    float cur_temperature;
    float cur_pressure;
    float cur_humidity;
    float cur_airQuality;
    float ave_temperature;
    float ave_pressure;
    float ave_humidity;
    float ave_airQuality;
    float min_temperature;
    float min_humidity;
    float min_pressure;
    float min_air_quality;
    float max_temperature;
    float max_humidity;
    float max_pressure;
    float max_air_quality;
    uint16_t total_readgings;
} EnvironmentMeasurement;

class EnvironmentSensor
{
public:
    bool Initialize();
    bool UpdateMeasurments();
    bool CheckStatus();
    EnvironmentMeasurement Measurments;

private:
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

    bool initializeBMx280();
    bool initializeBME680();
    bool checkIaqSensorStatus(void);
    bool TakeSample();
    float readTemperature();
    float readPressure();
    float readHumidity();
    float readAirQuality();
};