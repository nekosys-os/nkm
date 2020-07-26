using System;
using System.Collections.Generic;

namespace nkm.AST
{
    public class CommandTemplate
    {
        public struct ParameterDef
        {
            public string key;
            public string defaultValue;
            public string replacer;
        }

        private string template;

        private List<ParameterDef> parameterDefs;

        public CommandTemplate(string template, List<ParameterDef> parameterDefs)
        {
            this.template = template;
            this.parameterDefs = parameterDefs;
        }


        public string BuildCommand(Dictionary<string, string> parameterValues)
        {
            var str = template;
            foreach (var def in parameterDefs)
            {
                string value;
                if (!parameterValues.ContainsKey(def.key))
                    value = def.defaultValue;
                else
                    value = parameterValues[def.key];

                if (string.IsNullOrEmpty(value))
                    throw new Exception("Unresolved parameter '" + def.key + "'");

                str = str.Replace(def.replacer, value);
            }
            return str;
        }


    }
}
