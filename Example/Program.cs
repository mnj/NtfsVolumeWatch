using NtfsVolumeWatch;

namespace Example
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var manager = new Manager();
            manager.OnCreate += Manager_OnCreate;
            manager.OnRenameNewName += Manager_OnRenameNewName;


            manager.StartMonitoring("S:");

            // Wait for keypress to stop monitoring
            Console.ReadKey();

            manager.StopMonitoring();
        }

        private static void Manager_OnRenameNewName(string fileName)
        {
            Console.WriteLine($"File renamed: {fileName}");
        }

        private static void Manager_OnCreate(string fileName)
        {
            Console.WriteLine($"File created: {fileName}");
        }
    }
}
