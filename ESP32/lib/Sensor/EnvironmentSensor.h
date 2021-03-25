#include <bsec.h>
#include <BME280.h>
#include <enums.h>
#include <StoreHub.h>

#define SENSRO_MAX_AVERAGE_SAMPLES 60 * 24 // Maximum samples included in the calculating the average
const uint8_t bsec_config[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

typedef struct
{
    float cur_temperature;
    float cur_pressure;
    float cur_humidity;
    float cur_airQuality;
    double ave_temperature;
    double ave_pressure;
    double ave_humidity;
    double ave_airQuality;
    float min_temperature;
    float min_humidity;
    float min_pressure;
    float min_air_quality;
    float max_temperature;
    float max_humidity;
    float max_pressure;
    float max_air_quality;
    SensorCalibrationStatus calibrationStatus;
    float gas_resistance;
    uint16_t total_readgings;
    uint16_t total_airQuality_readings;
} EnvironmentMeasurement;

class EnvironmentSensor
{
public:
    bool Initialize(TwoWire &i2c);
    bool UpdateMeasurments();
    bool CheckStatus();
    bool Run();
    bool LoadState();
    bool StoreState();
    void ResetReport();
    SensorType sensorType;
    EnvironmentMeasurement Measurments;

private:
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
    float readAirQuality(SensorCalibrationStatus *status);
};