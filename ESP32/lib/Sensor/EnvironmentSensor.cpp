#include <EnvironmentSensor.h>

bool EnvironmentSensor::Initialize()
{
    printf("Initializing sensors.\r\n");
    bme680.begin(BME680_I2C_ADDR_PRIMARY, Wire);
    isBME680 = bme680.bme680Status != -3;
    if (isBME680)
    {
        printf("BME680 sensor found.\r\n");
        printf("BSEC library version %d.%d.%d.%d\r\n", bme680.version.major, bme680.version.minor, bme680.version.major_bugfix, bme680.version.minor_bugfix);
        initializeBME680();
        //free(bmx280);
        return true;
    }

    //free(bme680);
    if (bmx280.begin())
    {
        initializeBMx280();
        isBME280 = bmx280.isBME280;
        isBMP280 = !bmx280.isBME280;
        return true;
    }
    isBME280 = false;
    isBMP280 = false;
    printf("No sensor found.\r\n");
    //free(bmx280);
    return false;
}

bool EnvironmentSensor::CheckStatus()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return false;

    if (isBME680)
        return checkIaqSensorStatus();

    return true;
}

bool EnvironmentSensor::TakeSample()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return false;

    if (isBME680)
        return bme680.run();

    bmx280.takeForcedMeasurement();
    return true;
}

float EnvironmentSensor::readTemperature()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return -1;

    if (isBME680)
        return bme680.temperature;
    return bmx280.readTemperature();
}

float EnvironmentSensor::readHumidity()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return -1;

    if (isBME680)
        return bme680.humidity;
    return bmx280.readHumidity();
}

float EnvironmentSensor::readPressure()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return -1;

    if (isBME680)
        return bme680.pressure;
    return bmx280.readPressure();
}

float EnvironmentSensor::readAirQuality()
{
    if (!(isBME280 || isBME680 || isBMP280))
        return -1;

    if (isBME680)
        return bme680.iaq;
    return -1;
}

bool EnvironmentSensor::initializeBME680()
{
    bme680.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    return checkIaqSensorStatus();
}

bool EnvironmentSensor::initializeBMx280()
{
    bmx280.setSampling(BME280::sensor_mode::MODE_FORCED,
                       BME280::sensor_sampling::SAMPLING_X1,
                       BME280::sensor_sampling::SAMPLING_X1,
                       BME280::sensor_sampling::SAMPLING_X1,
                       BME280::sensor_filter::FILTER_OFF,
                       BME280::standby_duration::STANDBY_MS_1000);
    return true;
}

// Helper function definitions
bool EnvironmentSensor::checkIaqSensorStatus(void)
{
    if (bme680.status != BSEC_OK)
    {
        if (bme680.status < BSEC_OK)
            printf("BSEC error code : %d\r\n", bme680.status);
        else
            printf("BSEC warning code : %d\r\n", bme680.status);

        return false;
    }

    if (bme680.bme680Status != BME680_OK)
    {
        if (bme680.bme680Status < BME680_OK)
            printf("BME680 error code : %d\r\n", bme680.bme680Status);
        else
            printf("BME680 warning code : %d\r\n", bme680.bme680Status);

        return false;
    }

    return true;
}
