/*-------------------------------------
   GETPRNDC.C -- GetPrinterDC function
  -------------------------------------*/

#include <windows.h>

HDC GetPrinterDC (void)
{
     DWORD            dwNeeded, dwReturned ;
     HDC              hdc ;
     PRINTER_INFO_4 * pinfo4 ;
     PRINTER_INFO_5 * pinfo5 ; 
          EnumPrinters (PRINTER_ENUM_LOCAL, NULL, 4, NULL,
                        0, &dwNeeded, &dwReturned) ;

          pinfo4 = malloc (dwNeeded) ;

          EnumPrinters (PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE) pinfo4,
                        dwNeeded, &dwNeeded, &dwReturned) ;

          hdc = CreateDC (NULL, pinfo4->pPrinterName, NULL, NULL) ;

          free (pinfo4) ;
     
     return hdc ;   
}
