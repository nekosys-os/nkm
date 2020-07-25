using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public class ToolInvokeExpr : IExpression
    {
        public string ToolName { get; set; }

        public Dictionary<string, string> Parameters { get; } = new Dictionary<string, string>();
    }
}
