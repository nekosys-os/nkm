using nkm.AST;
using nkm.Logging;

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
            foreach (var expr in nekofile.Expressions)
            {
                if (expr is TargetDefExpr targetExpr && (target == string.Empty || targetExpr.Name == target))
                {
                    ExecuteTarget(targetExpr);
                }
            }

            return true;
        }

        private void ExecuteTarget(TargetDefExpr target)
        {
            LoggerFactory.Current.Log(LogLevel.Info, $"Building target {target.Name}...");
        }
    }
}
