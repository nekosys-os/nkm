using nkm.Logging;
using nkm.Parser;
using nkm.Runner;
using System;
using System.IO;
using System.Reflection;

namespace nkm
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var version = Assembly.GetExecutingAssembly().GetName().Version;
            Console.WriteLine($"NekoMake BuildSystem (nkm) v{version.Major}.{version.Minor}.{version.Revision} by Twometer");

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

            if (target == string.Empty)
                target = "default";

            LoggerFactory.Current.Log(LogLevel.Info, $"Building {target} in script {buildScript.FullName}...");

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
