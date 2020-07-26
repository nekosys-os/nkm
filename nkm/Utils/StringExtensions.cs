namespace nkm.Utils
{
    public static class StringExtensions
    {

        public static string Before(this string str, string seperator)
        {
            return str.Remove(str.IndexOf(seperator)).Trim();
        }

        public static string After(this string str, string seperator)
        {   
            return str.Substring(str.IndexOf(seperator) + seperator.Length).Trim();
        }

    }
}
