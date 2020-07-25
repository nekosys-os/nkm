using nkm.AST;
using nkm.Logging;
using System;
using System.Linq;
using System.Security;

namespace nkm.Runner
{
    public class NekofileRunner
    {
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
        }

        private ToolDefExpr ResolveTool(string name)
        {
            return nekofile.Expressions.Select(e => e as ToolDefExpr).Where(e => e?.Name == name).SingleOrDefault();
        }

        private string ResolveConstant(string name)
        {
            foreach (var expr in nekofile.Expressions)
                if (expr is ConstantExpr constant && constant.Name == name)
                    return constant.Value;
            return null;
        }

        private void ExecuteCommand(string command)
        {
            Console.WriteLine("$ " + command);
        }
    }
}
