using System;
using System.Diagnostics;

namespace nkm.Logging
{
    public class ConsoleLogger : ILogger
    {
        public void Log(LogLevel level, string message)
        {
            if (level == LogLevel.Debug && !Debugger.IsAttached)
                return;

            switch (level)
            {
                case LogLevel.Debug:
                    Console.ForegroundColor = ConsoleColor.DarkGray;
                    break;
                case LogLevel.Info:
                    Console.ForegroundColor = ConsoleColor.Cyan;
                    break;
                case LogLevel.Warning:
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    break;
                case LogLevel.Error:
                    Console.ForegroundColor = ConsoleColor.Red;
                    break;
            }

            Console.Write($"{level.ToPrefix().ToLower()} ");
            Console.ResetColor();

            Console.WriteLine(message);
        }
    }
}
