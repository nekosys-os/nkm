using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public class Nekofile
    {
        public List<IExpression> Expressions { get; } = new List<IExpression>();
    }
}
