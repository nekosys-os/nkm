using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.AST
{
    public enum InvokeStrategy
    {
        Single,
        Multi
    }

    public static class InvokeStrategies
    {
        public static InvokeStrategy Parse(string strategy)
        {
            switch (strategy.ToLower())
            {
                case "single": return InvokeStrategy.Single;
                case "multi": return InvokeStrategy.Multi;
                default:
                    throw new ArgumentException($"Unknown invoke strategy {strategy}");
            }
        }
    }
}
