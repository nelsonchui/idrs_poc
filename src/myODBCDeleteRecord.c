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



int main() {
  SQLHENV henv;
  SQLHDBC hdbc;
  SQLHSTMT hstmt;
  SQLRETURN rc; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;

  SQLINTEGER colNo;
  SQLLEN  rowCount;

  SQLUINTEGER CustomerID;
  SQLCHAR FirstName[LASTNAME_LEN];
  SQLCHAR LastName[FIRSTNAME_LEN];
  SQLCHAR Address[ADDRESS_LEN];
  SQLCHAR City[CITY_LEN];
  SQLLEN siCustomerID, siFirstName, siLastName, siAddress, siCity;

  // Return code values
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
  rc = SQLDriverConnect(hdbc, NULL, "DSN=PostgreDemoDB;", SQL_NTS,
                         outstr, sizeof(outstr), &outstrlen,
                         SQL_DRIVER_COMPLETE);
  if (SQL_SUCCEEDED(rc)) {
    printf("\nConnected\n\n");
    printf("Returned connection string was:\n%s\n", outstr);
    if (rc == SQL_SUCCESS_WITH_INFO) {
      printf("Driver reported the following diagnostics\n");
      extract_error("SQLDriverConnect", hdbc, SQL_HANDLE_DBC);
    }

  } else {
    fprintf(stderr, "\nFailed to connect\n");
    extract_error("SQLDriverConnect", hdbc, SQL_HANDLE_DBC);
    goto exit;
  }



// From this part onward is the SQLExecution.

// Step 1 - Allocate statement handle
//printf("Allocate statement handle\n");
rc = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
if (rc != SQL_SUCCESS) {
    printf("SQLAllocHandle() Return code: %i\n", rc);
    extract_error("SQLAllocHandle(hstmt)", hstmt, SQL_HANDLE_STMT);
    goto exit;
}
//printf("SQLAllocHandle() Return code: %i\n\n", rc);

// Step 2 - Execute statement
char sqlStatement[256] = "DELETE FROM customers WHERE CustomerID = 4";
printf("Execute SQL statement : %s \n", &sqlStatement);
rc = SQLExecDirect(hstmt, sqlStatement, SQL_NTS); 

if (rc != SQL_SUCCESS) {
    printf("SQLExecDirect() Return code: %i\n", rc);
    extract_error("SQLExecDirect(hstmt)", hstmt, SQL_HANDLE_STMT);
    goto exit;
}

// SQLRowCount only applicable for UPDATE, INSERT, DELETE
rc = SQLRowCount(hstmt, &rowCount);
if (rc != SQL_SUCCESS) {
    printf("SQLRowCount() Return code: %i\n", rc);
    extract_error("SQLRowCount(hstmt)", hstmt, SQL_HANDLE_STMT);
    goto exit;
}
printf ("%i row(s) deleted.\n", rowCount);

printf ("\nCompleted.\n");

exit:
  // Close the customer result set. 
  SQLCloseCursor(hstmt); 

  /* free up allocated handles */
  // Statement
  if (hstmt != SQL_NULL_HSTMT)
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
  // Connection
  if (hdbc != SQL_NULL_HDBC) {
      SQLDisconnect(hdbc);
      SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
  }
  // Environment
  if (henv != SQL_NULL_HENV)
      SQLFreeHandle(SQL_HANDLE_ENV, henv);

  return 0;

}