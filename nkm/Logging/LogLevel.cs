﻿using System;

namespace nkm.Logging
{
    public enum LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    }

    public static class LogLevelExtensions
    {

        public static string ToPrefix(this LogLevel logLevel)
        {
            switch (logLevel)
            {
                case LogLevel.Debug:
                    return "dbug";
                case LogLevel.Info:
                    return "info";
                case LogLevel.Warning:
                    return "warn";
                case LogLevel.Error:
                    return "error";
            }

            throw new ArgumentException($"Unknown log level {logLevel}");
        }

    }
}
