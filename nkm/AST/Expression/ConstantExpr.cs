using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public class ConstantExpr : IExpression
    {
        public string Name { get; set; }

        public string Value { get; set; }
    }
}
