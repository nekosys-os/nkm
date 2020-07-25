using nkm.AST;
using System.IO;

namespace nkm.Parser
{
    public class NekofileParser
    {
        private StreamReader reader;

        public Nekofile Document { get; private set; }

        public NekofileParser(FileInfo info)
        {
            reader = new StreamReader(info.FullName);
        }

        public bool Parse()
        {
            var lineNo = 0;

            while (reader.Peek() >= 0)
            {
                lineNo++;
                ParseLine(reader.ReadLine());
            }

            return true;
        }

        private void ParseLine(string line)
        {

        }
    }
}
