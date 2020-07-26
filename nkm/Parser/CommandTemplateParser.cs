using System;
using nkm.AST;
using nkm.Utils;
using System.Collections.Generic;
using static nkm.AST.CommandTemplate;

namespace nkm.Parser
{
    public class CommandTemplateParser
    {

        public static CommandTemplate Parse(string template)
        {
            List<ParameterDef> parameterDefs = new List<ParameterDef>();

            string tmp = template;
            var idx = 0;
            while (tmp.Contains("{"))
            {
                var def = new ParameterDef();
                idx += tmp.IndexOf("{") + 1;
                tmp = template.Substring(idx);

                if (!tmp.Contains("}"))
                    throw new Exception("Expected }");

                def.replacer = "{" + tmp.Before("}") + "}";

                if (tmp.Contains("="))
                {
                    def.key = tmp.Before("=");
                    def.defaultValue = tmp.After("=").Before("}");
                }
                else
                {
                    def.key = tmp.Before("}");
                }
                parameterDefs.Add(def);
            }

            return new CommandTemplate(template, parameterDefs);
        }

    }
}
