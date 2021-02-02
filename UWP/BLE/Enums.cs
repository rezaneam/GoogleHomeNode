using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Config_Tool___Google_Home_Node.BLE
{
    public class Enums
    {
        public enum ResetLevel
        {
            SoftReset, // Restart the Node 
            HardReset, // Erase the setting and then restart the node
            HardResetSafe // Erase Safely the setting and then restart the node
        }
    }
}
