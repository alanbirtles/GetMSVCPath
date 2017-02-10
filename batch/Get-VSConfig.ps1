$scriptRoot = Split-Path -Path $MyInvocation.MyCommand.Path
Add-Type -Path $scriptRoot\..\cs\VSConfig.cs
[VisualStudioSetup]::Main()
