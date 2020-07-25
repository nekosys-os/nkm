using nkm.AST;
using nkm.Logging;
using nkm.Utils;
using System;
using System.IO;
using System.Net.Mail;
using System.Security.Cryptography.X509Certificates;
using System.Text;

namespace nkm.Parser
{
    public class NekofileParser
    {
        public Nekofile Document { get; private set; }

        private StreamReader reader;

        public NekofileParser(FileInfo info)
        {
            reader = new StreamReader(info.FullName);
        }

        public bool Parse()
        {
            var lineNo = 0;

            Document = new Nekofile();

            try
            {
                while (reader.Peek() >= 0)
                {
                    lineNo++;
                    ParseLine(lineNo, reader.ReadLine());
                }
                CloseRemaining(0);
            }
            catch (Exception e)
            {
                LoggerFactory.Current.Log(LogLevel.Error, e.Message);
                return false;
            }

            return true;
        }

        private ToolDefExpr currentTool;
        private TargetDefExpr currentTarget;
        private ToolInvokeExpr currentToolInvoke;
        private string currentToolParam;

        private void ParseLine(int no, string line)
        {
            if (ShouldIgnore(line))
                return;

            var level = GetIndentLevel(line);

            if (level == 0)
            {
                CloseRemaining(level);

                if (line.Contains("="))
                {
                    Document.Expressions.Add(new ConstantExpr
                    {
                        Name = EnsureNotEmpty(no, line.Before("="), "Cannot have empty const name!"),
                        Value = EnsureNotEmpty(no, line.After("="), "Cannot have empty const value!")
                    });
                }
                else if (line.StartsWith("["))
                {
                    Expect(no, line, ']');

                    var name = line.Substring(1); // Skip the opening bracket
                    var strategy = InvokeStrategy.Multi;

                    if (name.Contains(","))
                    {
                        strategy = InvokeStrategies.Parse(name.After(",").Before("]"));
                        name = name.Before(",");
                    }
                    else
                    {
                        name = name.Before("]");
                    }

                    Document.Expressions.Add(currentTool = new ToolDefExpr { Name = EnsureNotEmpty(no, name, "Cannot have empty tool name!"), InvokeStrategy = strategy });
                }
                else if (line.Contains(":"))
                {
                    Document.Expressions.Add(currentTarget = new TargetDefExpr { Name = EnsureNotEmpty(no, line.Before(":"), "Cannot have empty target name!") });
                }
            }
            else if (level == 1)
            {
                CloseRemaining(level);
                line = line.Trim();

                if (currentTool != null)
                {
                    if (currentTool.CommandTemplate != null)
                        Error(no, "A tool can only hold a single command template!");
                    else currentTool.CommandTemplate = line;
                }
                else if (currentTarget != null)
                {
                    if (line.StartsWith("["))
                    {
                        Expect(no, line, ']');
                        currentTarget.Commands.Add(currentToolInvoke = new ToolInvokeExpr { ToolName = line.After("[").Before("]") });
                    }
                    else if (currentToolInvoke == null)
                    {
                        currentTarget.Commands.Add(new RawCommandExpr { Value = line });
                    }
                    else
                    {
                        Error(no, "Unexpected statement!");
                    }
                }
                else
                {
                    Error(no, "Dangling statement outside of tool or target!");
                }
            }
            else if (level >= 2)
            {
                line = line.Trim();
                if (currentToolInvoke == null)
                    Error(no, "Dangling statement outside of tool invocation!");

                if (line.Contains(":"))
                {
                    CloseRemaining(level);
                    var key = EnsureNotEmpty(no, line.Before(":"), "Cannot have empty parameter key!");
                    var value = EnsureNotEmpty(no, line.After(":"), "Cannot have empty first parameter value!");
                    if (currentToolInvoke == null)
                        Error(no, "Unexpected statement outside of tool invocation!");
                    currentToolInvoke.AddParameter(key, value);
                    currentToolParam = key;
                }
                else if (currentToolParam != null)
                {
                    if (!string.IsNullOrWhiteSpace(line))
                        currentToolInvoke.AddParameter(currentToolParam, line);
                }
                else
                {
                    Error(no, "Unexpected statement inside of tool invocation!");
                }
            }

        }

        private void CloseRemaining(int level)
        {
            if (level == 0)
            {
                currentTool = null;
                currentTarget = null;
            }

            if (level <= 1)
            {
                currentToolInvoke = null;
            }

            currentToolParam = null;
        }

        private bool ShouldIgnore(string line)
        {
            return line.Trim().StartsWith(";") || string.IsNullOrWhiteSpace(line);
        }

        private int GetIndentLevel(string line)
        {
            if (line.Length == 0) return 0;

            var baseChar = line[0];
            if (baseChar == '\t' || baseChar == ' ')
            {
                var num = 0;
                foreach (var c in line)
                    if (c == baseChar)
                        num++;
                    else break;

                if (baseChar == ' ')
                    num /= 4;

                return num;
            }
            return 0;
        }

        private string EnsureNotEmpty(int no, string text, string error)
        {
            if (string.IsNullOrWhiteSpace(text))
                Error(no, error);
            return text;
        }

        private void Expect(int no, string line, char chr)
        {
            if (!line.Contains(chr))
                Error(no, $"Expected '{chr}'");
        }

        private void Error(int line, string message)
        {
            throw new Exception($"Parser error: {message} (line {line})");
        }
    }
}
