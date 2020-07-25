using nkm.AST;
using System;
using System.Collections.Generic;
using System.Text;

namespace nkm.Runner
{
    public class NekofileRunner
    {
        private readonly Nekofile nekofile;

        public NekofileRunner(Nekofile nekofile)
        {
            this.nekofile = nekofile;
        }

        public bool Execute()
        {
            return true;
        }
    }
}
