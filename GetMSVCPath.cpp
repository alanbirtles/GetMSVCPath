#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <crtdbg.h>
#include <comip.h>
#include <comdef.h>
#include <comutil.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <Setup.Configuration.h>
#include "Helpers.h"

_COM_SMARTPTR_TYPEDEF(ISetupInstance, __uuidof(ISetupInstance));
_COM_SMARTPTR_TYPEDEF(ISetupInstance2, __uuidof(ISetupInstance2));
_COM_SMARTPTR_TYPEDEF(IEnumSetupInstances, __uuidof(IEnumSetupInstances));
_COM_SMARTPTR_TYPEDEF(ISetupConfiguration, __uuidof(ISetupConfiguration));
_COM_SMARTPTR_TYPEDEF(ISetupConfiguration2, __uuidof(ISetupConfiguration2));
_COM_SMARTPTR_TYPEDEF(ISetupHelper, __uuidof(ISetupHelper));
_COM_SMARTPTR_TYPEDEF(ISetupPackageReference, __uuidof(ISetupPackageReference));

module_ptr GetQuery(
  _Outptr_result_maybenull_ ISetupConfiguration** ppQuery
);

module_ptr GetQuery(
  _Outptr_result_maybenull_ ISetupConfiguration** ppQuery
)
{
  typedef HRESULT(CALLBACK* LPFNGETCONFIGURATION)(_Out_ ISetupConfiguration** ppConfiguration, _Reserved_ LPVOID pReserved);

  const WCHAR wzLibrary[] = L"Microsoft.VisualStudio.Setup.Configuration.Native.dll";
  const CHAR szFunction[] = "GetSetupConfiguration";

  // As with COM, make sure we return a NULL pointer on error.
  _ASSERT(ppQuery);
  *ppQuery = NULL;

  ISetupConfigurationPtr query;

  // Try to create the CoCreate the class; if that fails, likely no instances are registered.
  auto hr = query.CreateInstance(__uuidof(SetupConfiguration));
  if (SUCCEEDED(hr))
  {
    *ppQuery = query;
    return nullptr;
  }
  else if (REGDB_E_CLASSNOTREG != hr)
  {
    throw win32_exception(hr, "failed to create query class");
  }

  // We can otherwise attempt to load the library from the PATH.
  auto hConfiguration = ::LoadLibraryW(wzLibrary);
  if (!hConfiguration)
  {
    throw win32_exception(REGDB_E_CLASSNOTREG, "failed to load configuration library");
  }

  // Make sure the module is freed when it falls out of scope.
  module_ptr lib(&hConfiguration);

  auto fnGetConfiguration = reinterpret_cast<LPFNGETCONFIGURATION>(::GetProcAddress(hConfiguration, szFunction));
  if (!fnGetConfiguration)
  {
    throw win32_exception(CLASS_E_CLASSNOTAVAILABLE, "could not find the expected entry point");
  }

  hr = fnGetConfiguration(ppQuery, NULL);
  if (FAILED(hr))
  {
    throw win32_exception(hr, "failed to get query class");
  }

  return lib;
}

void checkResult( HRESULT hr )
{
  if ( FAILED( hr ) )
  {
    throw win32_exception( hr, "call failed" );
  }
}

using namespace std;

int wmain( int argc, wchar_t* argv[] )
{
  try
  {
    if ( argc != 2 )
    {
      wcout << "Visual studio version required" << endl;
      return 1;
    }
    size_t pos;
    wstring versionStr( argv[ 1 ] );
    int version = std::stoi( argv[ 1 ], &pos );
    if (pos != versionStr.size())
    {
      wcout << "Can't parse Visual studio version" << endl;
      return 1;
    }

    const wstring VCCompilerType( L"Component" );
    const wstring VCCompilerID( L"Microsoft.VisualStudio.Component.VC.Tools.x86.x64" );

    CoInitializer init;

    ISetupConfigurationPtr query;
    auto lib = GetQuery(&query);

    ISetupConfiguration2Ptr query2(query);
    IEnumSetupInstancesPtr e;

    checkResult( query2->EnumAllInstances(&e) );

    ISetupHelperPtr helper(query);

    ISetupInstance* pInstances[1] = {};
    auto hr = e->Next(1, pInstances, NULL);
    while (S_OK == hr)
    {
      // Wrap instance without AddRef'ing.
      ISetupInstancePtr instance(pInstances[0], false);
      bstr_t bstrVersion;
      checkResult( instance->GetInstallationVersion( bstrVersion.GetAddress() ) );
      ULONGLONG ullVersion;
      checkResult( helper->ParseVersion( bstrVersion, &ullVersion ) );
      // get the major version number
      ullVersion >>= 48L;
      if ( ullVersion == version )
      {
        ISetupInstance2Ptr instance2( instance );
        LPSAFEARRAY psa = NULL;
        checkResult( instance2->GetPackages( &psa ) );
        safearray_ptr psa_ptr( &psa );
        // Lock the SAFEARRAY to get the raw pointer array.
        checkResult( ::SafeArrayLock( psa ) );

        auto rgpPackages = reinterpret_cast< ISetupPackageReference** >( psa->pvData );
        auto cPackages = psa->rgsabound[ 0 ].cElements;

        if ( 0 != cPackages )
        {
          vector< ISetupPackageReference* > packages( rgpPackages, rgpPackages + cPackages );
          for ( auto pPackage : packages )
          {
            bstr_t bstrType;
            checkResult( pPackage->GetType( bstrType.GetAddress() ) );
            bstr_t bstrID;
            checkResult( pPackage->GetId( bstrID.GetAddress() ) );
            if ( VCCompilerType == wstring( bstrType ) && VCCompilerID == wstring( bstrID ) )
            {
              bstr_t bstrInstallationPath;
              instance2->GetInstallationPath( bstrInstallationPath.GetAddress() );
              wcout << bstrInstallationPath;
              return 0;
            }
          }
        }
      }

      hr = e->Next(1, pInstances, NULL);
    }

    if (FAILED(hr))
    {
      throw win32_exception(hr, "failed to enumerate all instances");
    }
  }
  catch (win32_exception& ex)
  {
    cerr << hex << "Error 0x" << ex.code() << ": " << ex.what() << endl;
    return 1;
  }
  catch (exception& ex)
  {
    cerr << "Error: " << ex.what() << endl;
    return 1;
  }

  return 1;
}

