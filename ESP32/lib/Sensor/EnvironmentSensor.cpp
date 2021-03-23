#include <EnvironmentSensor.h>

bool EnvironmentSensor::Initialize(TwoWire &i2c)
{
    bme680.begin(BME680_I2C_ADDR_SECONDARY, i2c);
    ;
    if (bme680.bme680Status == 0)
    {
        printf("BME680 sensor found.\r\n");
        printf("BSEC library version %d.%d.%d.%d\r\n",
               bme680.version.major, bme680.version.minor, bme680.version.major_bugfix, bme680.version.minor_bugfix);
        initializeBME680();
        sensorType = SensorType::BME680_Sensor;
        return true;
    }

    if (bmx280.begin(BME280_ADDRESS, &i2c))
    {
        initializeBMx280();
        sensorType = bmx280.isBME280 ? SensorType::BME280_Sensor : SensorType::BMP280_Sensor;
        return true;
    }
    sensorType = SensorType::No_Sensor;
    printf("No sensor found.\r\n");
    return false;
}

bool EnvironmentSensor::CheckStatus()
{
    if (sensorType == SensorType::No_Sensor)
        return false;

    if (sensorType == SensorType::BME680_Sensor)
        return checkIaqSensorStatus();

    return true;
}

bool EnvironmentSensor::Run()
{
    if (sensorType == SensorType::BME680_Sensor)
        return bme680.run();
    return true;
}

bool EnvironmentSensor::TakeSample()
{
    if (sensorType == SensorType::No_Sensor)
        return false;

    if (sensorType == SensorType::BME680_Sensor)
    {
        bme680.run();
        return true;
    }

    bmx280.takeForcedMeasurement();
    return true;
}

float EnvironmentSensor::readTemperature()
{
    if (sensorType == SensorType::No_Sensor)
        return -1;

    if (sensorType == SensorType::BME680_Sensor)
        return bme680.temperature;
    return bmx280.readTemperature();
}

float EnvironmentSensor::readHumidity()
{
    if (sensorType == SensorType::No_Sensor)
        return -1;

    if (sensorType == SensorType::BME680_Sensor)
        return bme680.humidity;
    return bmx280.readHumidity();
}

float EnvironmentSensor::readPressure()
{
    if (sensorType == SensorType::No_Sensor)
        return -1;

    if (sensorType == SensorType::BME680_Sensor)
        return bme680.pressure;
    return bmx280.readPressure();
}

float EnvironmentSensor::readAirQuality(SensorCalibrationStatus *status)
{
    if (sensorType == SensorType::No_Sensor)
        return -1;

    if (sensorType == SensorType::BME680_Sensor)
    {
        Measurments.gas_resistance = bme680.gasResistance;
        *status = (SensorCalibrationStatus)bme680.iaqAccuracy;
        return bme680.iaq;
    }

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

bool EnvironmentSensor::UpdateMeasurments()
{
    if (!TakeSample())
        return false;

    SensorCalibrationStatus status;
    Measurments.cur_temperature = readTemperature();
    Measurments.cur_humidity = readHumidity();
    Measurments.cur_pressure = readPressure();
    Measurments.cur_airQuality = readAirQuality(&status);
    if (Measurments.total_readgings > 0)
    {
        if (Measurments.min_temperature > Measurments.cur_temperature)
            Measurments.min_temperature = Measurments.cur_temperature;
        if (Measurments.max_temperature < Measurments.cur_temperature)
            Measurments.max_temperature = Measurments.cur_temperature;

        if (Measurments.min_humidity > Measurments.cur_humidity)
            Measurments.min_humidity = Measurments.cur_humidity;
        if (Measurments.max_humidity < Measurments.cur_humidity)
            Measurments.max_humidity = Measurments.cur_humidity;

        if (Measurments.min_pressure > Measurments.cur_pressure)
            Measurments.min_pressure = Measurments.cur_pressure;
        if (Measurments.max_pressure < Measurments.cur_pressure)
            Measurments.max_pressure = Measurments.cur_pressure;
    }
    else
    {
        Measurments.min_temperature = Measurments.cur_temperature;
        Measurments.max_temperature = Measurments.cur_temperature;

        Measurments.min_humidity = Measurments.cur_humidity;
        Measurments.max_humidity = Measurments.cur_humidity;

        Measurments.min_pressure = Measurments.cur_pressure;
        Measurments.max_pressure = Measurments.cur_pressure;
    }

    Measurments.ave_temperature =
        (Measurments.ave_temperature * Measurments.total_readgings + Measurments.cur_temperature) /
        (Measurments.total_readgings + 1);

    Measurments.ave_humidity =
        (Measurments.ave_humidity * Measurments.total_readgings + Measurments.cur_humidity) /
        (Measurments.total_readgings + 1);

    Measurments.ave_pressure =
        (Measurments.ave_pressure * Measurments.total_readgings + Measurments.cur_pressure) /
        (Measurments.total_readgings + 1);

    Measurments.total_readgings++;

    Measurments.calibrationStatus = status;
    if (status >= SensorCalibrationStatus::MEDIUM_ACCURACY)
    {
        if (Measurments.total_airQuality_readings > 0)
        {
            if (Measurments.min_air_quality > Measurments.cur_airQuality)
                Measurments.min_air_quality = Measurments.cur_airQuality;
            if (Measurments.max_air_quality < Measurments.cur_airQuality)
                Measurments.max_air_quality = Measurments.cur_airQuality;
        }
        else
        {
            Measurments.min_air_quality = Measurments.cur_airQuality;
            Measurments.max_air_quality = Measurments.cur_airQuality;
        }

        Measurments.ave_airQuality =
            (Measurments.ave_airQuality * Measurments.total_airQuality_readings + Measurments.cur_airQuality) /
            (Measurments.total_airQuality_readings + 1);

        Measurments.total_airQuality_readings++;
    }

    return true;
}