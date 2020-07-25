using System.Collections.Generic;

namespace nkm.AST
{
    public class TargetDefExpr : IExpression
    {
        public string Name { get; set; }

        public List<IExpression> Commands { get; } = new List<IExpression>();
    }
}
