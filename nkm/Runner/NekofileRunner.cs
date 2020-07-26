using nkm.AST;
using nkm.Logging;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace nkm.Runner
{
    public class NekofileRunner
    {

        private const string InputParam = "input";
        private const string OutputParam = "output";

        private readonly Nekofile nekofile;

        public NekofileRunner(Nekofile nekofile)
        {
            this.nekofile = nekofile;
        }

        public bool Execute(string target)
        {
            try
            {
                foreach (var expr in nekofile.Expressions)
                {
                    if (expr is TargetDefExpr targetExpr && (target == string.Empty || targetExpr.Name == target))
                    {
                        ExecuteTarget(targetExpr);
                    }
                }

                return true;
            }
            catch (Exception e)
            {
                LoggerFactory.Current.Log(LogLevel.Error, e.Message);
                LoggerFactory.Current.Log(LogLevel.Debug, e.ToString());
                return false;
            }
        }

        private void ExecuteTarget(TargetDefExpr target)
        {
            LoggerFactory.Current.Log(LogLevel.Info, $"Building target {target.Name}...");

            foreach (var cmd in target.Commands)
            {
                if (cmd is RawCommandExpr rawCmd)
                {
                    ExecuteCommand(rawCmd.Value);
                }
                else if (cmd is ToolInvokeExpr invokeCmd)
                {
                    InvokeTool(invokeCmd);
                }
            }
        }

        private void InvokeTool(ToolInvokeExpr expr)
        {
            var toolDef = ResolveTool(expr.ToolName);
            var template = toolDef.CommandTemplate;

            if (toolDef.InvokeStrategy == InvokeStrategy.Single)
            {
                var parameters = new Dictionary<string, string>();

                foreach (var kvp in expr.Parameters)
                    parameters[kvp.Key] = BuildParameterValueSingle(kvp.Key, kvp.Value);

                LoadConstants(parameters);
                ExecuteCommand(template.BuildCommand(parameters));
            }
            else if (toolDef.InvokeStrategy == InvokeStrategy.Multi)
            {
                var input = expr.Parameters.ContainsKey(InputParam) ? expr.Parameters[InputParam] : null;
                var output = expr.Parameters.ContainsKey(OutputParam) ? expr.Parameters[OutputParam] : null;

                List<string> inputs = new List<string>();
                foreach (var inp in input)
                    ExpandPath(inp, inputs);

                foreach (var i in inputs)
                {
                    var parameters = new Dictionary<string, string>();
                    parameters.Add(InputParam, i);

                    if (output != null)
                    {
                        var fi = new FileInfo(i);
                        parameters.Add(OutputParam, output.Single().Replace("~", fi.Name.Remove(fi.Name.Length - fi.Extension.Length)));
                    }

                    foreach (var kvp in expr.Parameters)
                        if (kvp.Key != InputParam && kvp.Key != OutputParam)
                            parameters[kvp.Key] = string.Join(' ', kvp.Value);

                    LoadConstants(parameters);
                    ExecuteCommand(template.BuildCommand(parameters));
                }
            }
        }



        private string BuildParameterValueSingle(string key, List<string> values)
        {
            if (key.Equals(InputParam, StringComparison.OrdinalIgnoreCase))
            {
                List<string> newValues = new List<string>();
                foreach (var val in values)
                    ExpandPath(val, newValues);
                return string.Join(' ', newValues);
            }
            else
            {
                return string.Join(' ', values);
            }
        }

        private void ExpandPath(string path, List<string> output)
        {
            var fullPath = Path.Combine(Environment.CurrentDirectory, path).Replace("\\", "/");

            var pattern = fullPath.Split("/").Last();
            var basePath = fullPath.Remove(fullPath.Length - pattern.Length);

            var dirInf = new DirectoryInfo(basePath);
            if (!dirInf.Exists)
                throw new Exception($"Directory does not exist ({basePath})");

            var option = pattern.Contains("**") ? SearchOption.AllDirectories : SearchOption.TopDirectoryOnly;

            foreach (var fi in dirInf.EnumerateFiles("*", option))
            {
                if (pattern.EndsWith(".*"))
                    output.Add(fi.FullName);
                else if (pattern.EndsWith(fi.Extension))
                    output.Add(fi.FullName);
            }
        }

        private ToolDefExpr ResolveTool(string name)
        {
            return nekofile.Expressions.Select(e => e as ToolDefExpr).Where(e => e?.Name == name).SingleOrDefault();
        }

        private void LoadConstants(Dictionary<string, string> parameters)
        {
            foreach (var e in nekofile.Expressions)
                if (e is ConstantExpr constant)
                    parameters[constant.Name] = constant.Value;
        }

        private void ExecuteCommand(string command)
        {
            var startInfo = new ProcessStartInfo();

            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                startInfo.FileName = "cmd.exe";
                startInfo.Arguments = "/C " + command;
            }
            else
            {
                startInfo.FileName = "sh";
                var escapedArgs = command.Replace("\"", "\\\""); 
                startInfo.Arguments = $"-c \"{escapedArgs}\"";
            }

            var process = Process.Start(startInfo);
            process.WaitForExit();
            if (process.ExitCode != 0)
                throw new Exception($"The command '{command}' terminated with non-zero exit code {process.ExitCode}");
        }
    }
}
