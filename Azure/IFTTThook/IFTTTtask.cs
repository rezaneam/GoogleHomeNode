using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using Microsoft.Azure.Devices;

namespace IFTTThook
{

    public class Payload
    {
        public Payload(string userID, string lang, string key)
        {

            Language = lang;
            UserID = userID;
            Key = key;
        }

        public string Language;
        public string UserID;
        public string Key;
    }
    public static class IFTTTtask
    {
        const string IoTHunName = "Type IoT Hub Name";
        const string IoTDeviceName = "Type IoT Device Name";
        const string IoTSharedAccessKey = "Type IoT Share Access Key";

        [FunctionName("IFTTTtask")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Function, "get", "post", Route = null)] HttpRequest req,
            ILogger log)
        {

            string methodName = req.Query["action"];

            string requestBody = await new StreamReader(req.Body).ReadToEndAsync();
            dynamic data = JsonConvert.DeserializeObject(requestBody);

            methodName ??= data?.Action;
            if (!string.IsNullOrEmpty(methodName))
            {
                string userID = data?.UserID ?? "";
                string language = data?.Language ?? "";
                string key = data?.Key ?? "";

                var command = new CloudToDeviceMethod(methodName) { ResponseTimeout = TimeSpan.FromSeconds(30) };

                string payload = JsonConvert.SerializeObject(new Payload(userID, language ,key));
                command.SetPayloadJson(payload);

                string connectionString = $"HostName={IoTHunName}.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey={IoTSharedAccessKey}";

                ServiceClient serviceClient = ServiceClient.CreateFromConnectionString(connectionString, TransportType.Amqp);
                
                await serviceClient.InvokeDeviceMethodAsync(IoTDeviceName, command);
            }


            methodName = methodName ?? data?.name;

            string responseMessage = string.IsNullOrEmpty(methodName)
                ? "You request is well received. Pass an action in the query string."
                : $"{methodName} is passed. This HTTP triggered function executed successfully.";

            return new OkObjectResult(responseMessage);
        }
    }
}
