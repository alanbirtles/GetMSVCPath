using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Setup.Configuration;


namespace Microsoft.VisualStudio.Setup.Configuration
{
    [CompilerGenerated]
    [Flags]
    [TypeIdentifier("310100ba-5f84-4103-abe0-e8132ae862d9", "Microsoft.VisualStudio.Setup.Configuration.InstanceState")]
    [ComVisible(true)]
    public enum InstanceState : uint
    {
        None = 0,
        Local = 1,
        Registered = 2,
        NoRebootRequired = 4,
        NoErrors = 8,
        Complete = 4294967295
    }

    [CompilerGenerated]
    [Guid("6380BCFF-41D3-4B2E-8B2E-BF8A6810C848")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface IEnumSetupInstances
    {
        void Next([MarshalAs(UnmanagedType.U4)] [In] int celt,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Interface)] [Out] ISetupInstance[] rgelt,
            [MarshalAs(UnmanagedType.U4)] out int pceltFetched);
    }


    [CompilerGenerated]
    [Guid("42843719-DB4C-46C2-8E7C-64F1816EFD5B")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface ISetupConfiguration
    {
    }

    [CompilerGenerated]
    [Guid("26AAB78C-4A60-49D6-AF3B-3C35BC93365D")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface ISetupConfiguration2 : ISetupConfiguration
    {
        [SpecialName]
        [MethodImpl(MethodCodeType = MethodCodeType.Runtime)]
        void _VtblGap1_3();

        [return: MarshalAs(UnmanagedType.Interface)]
        IEnumSetupInstances EnumAllInstances();
    }


    [CompilerGenerated]
    [Guid("B41463C3-8866-43B5-BC33-2B0676F7F42E")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface ISetupInstance
    {
        [SpecialName]
        [MethodImpl(MethodCodeType = MethodCodeType.Runtime)]
        void _VtblGap1_4();

        [return: MarshalAs(UnmanagedType.BStr)]
        string GetInstallationVersion();
    }

    [CompilerGenerated]
    [Guid("89143C9A-05AF-49B0-B717-72E218A2185C")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface ISetupInstance2 : ISetupInstance
    {
        [return: MarshalAs(UnmanagedType.BStr)]
        string GetInstanceId();

        [SpecialName]
        [MethodImpl(MethodCodeType = MethodCodeType.Runtime)]
        void _VtblGap1_2();

        [return: MarshalAs(UnmanagedType.BStr)]
        string GetInstallationPath();

        [SpecialName]
        [MethodImpl(MethodCodeType = MethodCodeType.Runtime)]
        void _VtblGap2_4();

        [return: MarshalAs(UnmanagedType.U4)]
        InstanceState GetState();

        [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
        ISetupPackageReference[] GetPackages();

        ISetupPackageReference GetProduct();
    }

    [CompilerGenerated]
    [Guid("DA8D8A16-B2B6-4487-A2F1-594CCCCD6BF5")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [TypeIdentifier]
    [ComImport]
    public interface ISetupPackageReference
    {
        [return: MarshalAs(UnmanagedType.BStr)]
        string GetId();

        [SpecialName]
        [MethodImpl(MethodCodeType = MethodCodeType.Runtime)]
        void _VtblGap1_4();

        [return: MarshalAs(UnmanagedType.BStr)]
        string GetType();
    }
}

public static class VisualStudioSetup
{
    public static int Main()
    {
        try
        {
            var query =
                (ISetupConfiguration2)
                Activator.CreateInstance(Marshal.GetTypeFromCLSID(new Guid("177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D")));
            var enumSetupInstances = query.EnumAllInstances();
            var rgelt = new ISetupInstance2[1];
            int pceltFetched;
            do
            {
                // ISSUE: reference to a compiler-generated method
                enumSetupInstances.Next(1, rgelt, out pceltFetched);
                if (pceltFetched > 0)
                    PrintInstance(rgelt[0]);
            } while (pceltFetched > 0);
            return 0;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine("Error 0x{0:x8}: {1}", ex.HResult, ex.Message);
            return ex.HResult;
        }
    }


    private static void PrintInstance(ISetupInstance2 setupInstance2)
    {
        Console.WriteLine("InstallationPath: {0}", setupInstance2.GetInstallationPath());
        Console.WriteLine("Product: {0}", setupInstance2.GetProduct().GetId());
        foreach (var package in setupInstance2.GetPackages())
        {
            if (package.GetType() != "Exe") continue;;
            var id = package.GetId();
            if (id.IndexOf("SDK", StringComparison.Ordinal) == -1) continue;
            var parts = id.Split('_');
            if (parts.Length < 2) continue;
            var sdkVer = parts[1];
            char[] chars = { '1', '0', '8' };
            if (sdkVer.IndexOfAny(chars) == -1) continue;
            Console.WriteLine("SDK: {0}", sdkVer);
        }
        Console.WriteLine();
    }
}
