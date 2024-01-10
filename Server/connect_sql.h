//connect_sql.h
#pragma once

#include <sql.h>
#include <sqlext.h>

SQLHENV ConnectToDatabase(SQLHDBC& hdbc, SQLHENV& henv);
void DisconnectFromDatabase(SQLHDBC hdbc, SQLHENV henv);