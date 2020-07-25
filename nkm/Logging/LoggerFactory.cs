﻿using System.Diagnostics;

namespace nkm.Logging
{
    public class LoggerFactory
    {
        public static ILogger Current { get; private set; } = Debugger.IsAttached ? (ILogger)new DebugLogger() : new ConsoleLogger();

        public static void ForceLogger(ILogger logger)
        {
            Current = logger;
        }
    }
}
