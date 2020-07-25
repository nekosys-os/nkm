using System;
using System.Diagnostics;

namespace nkm.Logging
{
    public class DebugLogger : ILogger
    {
        public void Log(LogLevel level, string message)
        {
            Debug.WriteLine($"[{DateTime.Now.ToLongTimeString()}] [{level.ToPrefix()}] {message}");
        }
    }
}
