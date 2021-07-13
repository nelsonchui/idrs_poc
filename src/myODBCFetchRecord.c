#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>

#define CUSTOMERID_LEN  2
#define LASTNAME_LEN  255
#define FIRSTNAME_LEN 255
#define ADDRESS_LEN 255
#define CITY_LEN  255



/*
 * see Retrieving ODBC Diagnostics
 * for a definition of extract_error().
 */
void extract_error(
    char *fn,
    SQLHANDLE handle,
    SQLSMALLINT type)
{
    SQLINTEGER   i = 0;
    SQLINTEGER   native;
    SQLCHAR      state[ 7 ];
    SQLCHAR      text[256];
    SQLSMALLINT  len;
    SQLRETURN    ret;

    fprintf(stderr,
            "\n"
            "The driver reported the following diagnostics whilst running "
            "%s\n\n",
            fn);

    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                            sizeof(text), &len );
        if (SQL_SUCCEEDED(ret))
            printf("%s:%ld:%ld:%s\n", state, i, native, text);
    }
    while( ret == SQL_SUCCESS );
}



void main() {
  SQLHENV henv;
  SQLHDBC hdbc;
  SQLHSTMT hstmt;
  SQLRETURN ret; /* ODBC API return status */
  SQLRETURN retcode; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  
  SQLRETURN rc;

  SQLINTEGER colNo;

  SQLUINTEGER CustomerID;
  SQLCHAR FirstName[LASTNAME_LEN];
  SQLCHAR LastName[FIRSTNAME_LEN];
  SQLCHAR Address[ADDRESS_LEN];
  SQLCHAR City[CITY_LEN];
  SQLLEN siCustomerID, siFirstName, siLastName, siAddress, siCity;


  printf("\nSQL_SUCCESS: %i\n", SQL_SUCCESS);
  printf("SQL_SUCCESS_WITH_INFO: %i\n", SQL_SUCCESS_WITH_INFO);
  printf("SQL_ERROR: %i\n", SQL_ERROR);
  printf("SQL_INVALID_HANDLE: %i\n", SQL_INVALID_HANDLE);
  printf("SQL_NO_DATA: %i\n", SQL_NO_DATA);
  printf("SQL_NEED_DATA: %i\n", SQL_NEED_DATA);
  printf("SQL_STILL_EXECUTING: %i\n", SQL_STILL_EXECUTING);
  printf("SQL_NTS: %i\n\n", SQL_NTS);


  /* Allocate an environment handle */
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
  /* We want ODBC 3 support */
  SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
  /* Allocate a connection handle */
  SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
  /* Connect to the DSN mydsn */
  ret = SQLDriverConnect(hdbc, NULL, "DSN=PostgreDemoDB;", SQL_NTS,
                         outstr, sizeof(outstr), &outstrlen,
                         SQL_DRIVER_COMPLETE);
  if (SQL_SUCCEEDED(ret)) {
    printf("Connected\n");
    printf("Returned connection string was:\n\t%s\n", outstr);
    if (ret == SQL_SUCCESS_WITH_INFO) {
      printf("Driver reported the following diagnostics\n");
      extract_error("SQLDriverConnect", hdbc, SQL_HANDLE_DBC);
    }

  } else {
    fprintf(stderr, "Failed to connect\n");
    extract_error("SQLDriverConnect", hdbc, SQL_HANDLE_DBC);
    goto exit;
  }



// From this part onward is the SQLExecution.

// Step 1 - Allocate statement handle
printf("Allocate statement handle\n");
rc = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
printf("SQLAllocHandle() Return code: %i\n\n", rc);

// Step 2 - Execute statement
char sqlStatement[256] = "SELECT * FROM customers";
printf("Execute SQL statement : %s \n\n", &sqlStatement);
rc = SQLExecDirect(hstmt, sqlStatement, SQL_NTS); 
printf("SQLExecDirect() Return code: %i\n\n", rc);

if (rc != SQL_SUCCESS) {
    extract_error("SQLExecDirect(hstmt)", hstmt, SQL_HANDLE_STMT);
    goto exit;
}

// Step 3 - Fetch and display the customer data. Code to check if rc equals 
 
while ((rc = SQLFetch(hstmt)) != SQL_NO_DATA && rc!= SQL_ERROR) { 
//while (rc == SQL_SUCCESS) { 
    
   // Display the customer data. Code not shown. 
    memset (FirstName, ' ', FIRSTNAME_LEN);
	memset (LastName, ' ', LASTNAME_LEN);
	memset (Address, ' ', ADDRESS_LEN);
	memset (City, ' ', CITY_LEN);

    colNo=1;

    // Get record data into variables
    rc = SQLGetData(hstmt, colNo++, SQL_C_ULONG,
                                    &CustomerID, 0, &siCustomerID);
    rc = SQLGetData(hstmt, colNo++, SQL_C_CHAR,
                                    FirstName, FIRSTNAME_LEN, &siFirstName);
    rc = SQLGetData(hstmt, colNo++, SQL_C_CHAR,
                                    LastName, LASTNAME_LEN, &siLastName);
    rc = SQLGetData(hstmt, colNo++, SQL_C_CHAR,
                                    Address, ADDRESS_LEN, &siAddress);
	rc = SQLGetData(hstmt, colNo++, SQL_C_CHAR,
                                    City, CITY_LEN, &siCity);
    if (rc == SQL_ERROR) {
        goto exit;
    }
    // Print the record
    printf("%i, %.10s, %.10s, %.10s, %.10s\n",
        CustomerID, FirstName, LastName, Address, City);

    
} 

  printf ("\nComplete.\n");

exit:
  // Close the customer result set. 
  SQLCloseCursor(hstmt); 
  SQLDisconnect(hdbc);               /* disconnect from driver */
  /* free up allocated handles */
  SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
  SQLFreeHandle(SQL_HANDLE_ENV, henv);
}