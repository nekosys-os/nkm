using nkm.Logging;
using nkm.Parser;
using nkm.Runner;
using System;
using System.IO;

namespace nkm
{
    public class Program
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("NekoMake BuildSystem (nkm) v0.1.0 by Twometer");

            LoggerFactory.ForceLogger(new ConsoleLogger());
            
            RunBuilder(args.Length == 0 ? string.Empty : args[0]);
        }

        private static FileInfo FindBuildScript()
        {
            return new FileInfo(Path.Combine(Environment.CurrentDirectory, "Nekofile"));
        }

        private static void RunBuilder(string target)
        {
            if (!EnsureBuildScript(out var buildScript))
                return;

            var parser = new NekofileParser(buildScript);
            if (!parser.Parse())
            {
                LoggerFactory.Current.Log(LogLevel.Error, "Invalid build script!");
                return;
            }

            var targetName = target == string.Empty ? "all targets" : target;
            LoggerFactory.Current.Log(LogLevel.Info, $"Building {targetName} in script {buildScript.FullName}...");

            var runner = new NekofileRunner(parser.Document);
            if (!runner.Execute(target))
            {
                LoggerFactory.Current.Log(LogLevel.Error, "Build failed!");
                return;
            }
        }

        private static bool EnsureBuildScript(out FileInfo buildScript)
        {
            buildScript = FindBuildScript();
            if (!buildScript.Exists)
            {
                LoggerFactory.Current.Log(LogLevel.Error, "Nekofile was not found in current directory.");
                return false;
            }
            return true;
        }

    }
}
