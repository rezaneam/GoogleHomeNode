#include <EnvironmentSensor.h>

bool EnvironmentSensor::Initialize(TwoWire &i2c, bool verboseMode)
{
    isVerbose = verboseMode;
    isHDC1080Exist = hdc1080.Initialize(i2c);
    if (isHDC1080Exist)
    {
        if (isVerbose)
            printf(">> Sensor : HDC1080 sensor found.\r\n");
        initializeHDC1080();
    }

    bme680.begin(BME680_I2C_ADDR_SECONDARY, i2c);

    if (bme680.bme680Status == 0)
    {
        if (isVerbose)
        {
            printf(">> Sensor : BME680 sensor found.\r\n");
            printf(">> Sensor : BSEC library version %d.%d.%d.%d\r\n",
                   bme680.version.major, bme680.version.minor, bme680.version.major_bugfix, bme680.version.minor_bugfix);
        }
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
    if (isVerbose)
        printf(">> Sensor : No sensor found.\r\n");
    return false;
}

bool EnvironmentSensor::tryFindBME680(TwoWire &i2c)
{

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

bool EnvironmentSensor::LoadState()
{
    if (sensorType != SensorType::BME680_Sensor)
        return true;
    if (!HasBSECstate())
        return false;
    uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
    if (!GetBSECstate(bsecState, BSEC_MAX_STATE_BLOB_SIZE))
        return false;
    bme680.setState(bsecState);
    return checkIaqSensorStatus();
}

bool EnvironmentSensor::StoreState()
{
    if (sensorType != SensorType::BME680_Sensor)
        return true;
    if (bme680.iaqAccuracy < 3)
    {
        if (isVerbose)
            printf(">> Sensor : Updating BSEC state is ignore due to low accuracy. [%d].\r\n", bme680.iaqAccuracy);
        return false; // Do not store if high accuracy is not acheived.
    }

    uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
    bme680.getState(bsecState);
    printf(">> Sensor : Updating the BSEC state.\r\n");
    WriteFlashBSECstate(bsecState, BSEC_MAX_STATE_BLOB_SIZE);
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
        Measurments.VOC = bme680.breathVocEquivalent;
        Measurments.CO2 = bme680.co2Equivalent;
        *status = (SensorCalibrationStatus)bme680.iaqAccuracy;
        return bme680.iaq;
    }

    return -1;
}

bool EnvironmentSensor::initializeBME680()
{
    bme680.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    bme680.setConfig(bsec_config);
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

void EnvironmentSensor::initializeHDC1080()
{
    hdc1080.Setup(
        HDC1080::HDC1080_MeasurementResolution::HDC1080_RESOLUTION_11BIT,
        HDC1080::HDC1080_MeasurementResolution::HDC1080_RESOLUTION_11BIT);
}
// Helper function definitions
bool EnvironmentSensor::checkIaqSensorStatus(void)
{
    if (bme680.status != BSEC_OK)
    {
        if (isVerbose)
        {
            if (bme680.status < BSEC_OK)
                printf(">> Sensor : BSEC error code : %d\r\n", bme680.status);
            else
                printf(">> Sensor : BSEC warning code : %d\r\n", bme680.status);
        }

        return false;
    }

    if (bme680.bme680Status != BME680_OK)
    {
        if (isVerbose)
        {
            if (bme680.bme680Status < BME680_OK)
                printf(">> Sensor : BME680 error code : %d\r\n", bme680.bme680Status);
            else
                printf(">> Sensor : BME680 warning code : %d\r\n", bme680.bme680Status);
        }

        return false;
    }

    return true;
}

void EnvironmentSensor::ResetReport()
{
    if (isVerbose)
        printf(">> Sensor : Reseting the daily report.\r\n");
    Measurments.total_readgings = 0;
    Measurments.total_airQuality_readings = 0;
}

bool EnvironmentSensor::UpdateMeasurments()
{
    if (!TakeSample())
        return false;

    SensorCalibrationStatus status;
    if (isHDC1080Exist)
    {
        Measurments.cur_temperature = (float)hdc1080.ReadTemperature();
        printf(">> Sensor. TI: HDC1080 %2.3f Bosch %2.3f\r\n", Measurments.cur_temperature, readTemperature());
    }
    else
        Measurments.cur_temperature = readTemperature();
    Measurments.cur_humidity = readHumidity();
    Measurments.cur_pressure = readPressure();
    Measurments.cur_airQuality = readAirQuality(&status);

    // Reseting the averaging/Min/Max if more contains more than 1 day sample
    if (Measurments.total_readgings > SENSRO_MAX_AVERAGE_SAMPLES)
        ResetReport();

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