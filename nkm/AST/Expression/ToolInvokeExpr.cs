using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public class ToolInvokeExpr : IExpression
    {
        public string ToolName { get; set; }

        public Dictionary<string, List<string>> Parameters { get; } = new Dictionary<string, List<string>>();

        public void AddParameter(string key, string value)
        {
            if (!Parameters.ContainsKey(key))
                Parameters[key] = new List<string>();

            Parameters[key].Add(value);
        }
    }
}
