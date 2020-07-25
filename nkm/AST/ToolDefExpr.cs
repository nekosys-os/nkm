﻿using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public class ToolDefExpr : IExpression
    {
        public string Name { get; set; }

        public string CommandTemplate { get; set; }

        public InvokeStrategy InvokeStrategy { get; set; }
    }
}
